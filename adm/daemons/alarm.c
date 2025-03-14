// /adm/daemons/alarm.c
// Schedule and execute events. The alarms are read from /adm/etc/alarms/*.txt,
// but overrideable by the Config daemon.
//
// Types of alarms and their patterns:
// B = Boot - s
// O = Once - y-m-D@H:M
// H = Hourly - M
// D = Daily - H:M
// W = Weekly - D@H:M
// Y = Yearly - m-D@H:M
//
// When an alarm is called on a target file/object, the function is called with
// the alarm as an argument. The function should be defined in the target file.
// Also, ensure that you are inheriting CLASS_ALARM in the target file, so that
// the function can access the alarm information, including the arguments.
//
// Created:     2024/02/25: Gesslar
// Last Change: 2024/02/25: Gesslar
//
// 2024/02/25: Gesslar - Created

#include <daemons.h>
#include <classes.h>

inherit STD_DAEMON;
inherit CLASS_ALARM;

// Functions
void reload_alarms();
private nomask parse_alarm_in_file(string file);
string *parse_line(string line);
class Alarm create_alarm(string *parts, int silent);
int calculate_alarm_time(class Alarm alarm, int next);
private int next_minute_start();
int validate_alarm(class Alarm alarm, int silent);

// Variables
private nomask class Alarm *alarms = ({});
private nosave int cid;

void setup() {
  int next_minute = next_minute_start();

  set_persistent(1);

  cid = call_out_walltime("poll_alarms", next_minute - time());

  slot(SIG_SYS_BOOT, "execute_boot_alarms");
}

void post_restore() {
  if(!sizeof(alarms))
    reload_alarms();
}

void reload_alarms() {
  string alarm_path = mud_config("ALARMS_PATH");
  string alarm_file, *alarm_files;
  int next_minute = next_minute_start();

  alarms = ({});

  alarm_files = get_dir(alarm_path + "*.txt");
  alarm_files = map(alarm_files, (: $2 + $1 :), alarm_path);

  if(!sizeof(alarms)) {
    foreach(alarm_file in alarm_files)
      parse_alarm_in_file(alarm_file);
  }

  save_data();
}

varargs int add_once(string master, string pattern, string file, string func, mixed args...) {
  class Alarm alarm;
  mixed err;
  object ob;
  int time;
  string *parts;

  if(!master || !pattern || !file || !func) {
    throw("Invalid arguments");
    return 0;
  }

  parts = ({
    "O",
    pattern,
    master,
    file,
    func,
    args
  });

  alarm = create_alarm(parts, 1);

  if(alarm == null)
    return 0;

  alarms += ({ alarm });
  save_data();
  return 1;
}

void poll_alarms() {
  int i, sz, now, next, next_minute, until_next_poll;
  mixed err;
  object ob;

  now = time();
  next_minute = next_minute_start();
  until_next_poll = next_minute - now + 1 ; // Adjust to ensure it's right after the minute starts.

  if(find_call_out(cid) != -1)
    remove_call_out(cid);

  cid = call_out_walltime("poll_alarms", until_next_poll);

  // We need to run sizeof alarms times, because we might remove an alarm
  for(i = 0; i < sizeof(alarms); i++) {
    class Alarm alarm = alarms[i];
    // Initially check for the current or immediate next occurrence (not forced future)
    int next_current = calculate_alarm_time(alarm, 0);
    // Then force check for the strictly next occurrence (future)
    int next_future = calculate_alarm_time(alarm, 1);

    // Decide to trigger based on the immediate next occurrence time
    if(now >= next_current && (now <= next_current + 59) && alarm.last_run < next_current) {
      // Execute the alarm, considering the grace period and ensuring it hasn't been executed for this occurrence.
      alarm.last_run = now ; // Update last_run to mark this execution.
      call_out("execute_alarm", 0.01, alarm) ; // Schedule the alarm execution.

      // Now remove the alarm if it's a one-time alarm
      if(alarm.type == "O") {
        alarms = splice(alarms, i, 1);
        i-- ; // Decrement the index to account for the removed alarm
      }
    } else {
      if(alarm.type == "O") {
        if(now > next_current) {
          alarms = splice(alarms, i, 1);
          i-- ; // Decrement the index to account for the removed alarm
        }
      }
    }
  }
}

void execute_alarm(class Alarm alarm) {
  mixed err;
  object ob;

  if(!cfile_exists(alarm.file)) {
    log_file("system/alarm", "[%s] File %s does not exist\n", ctime(), alarm.file);
    return;
  }

  if(err = catch(ob = load_object(alarm.file))) {
    log_file("system/alarm", "[%s] Unable to load file %s: %O\n", ctime(), alarm.file, err);
    return;
  }

  if(!ob) {
    log_file("system/alarm", "[%s] Unable to load file %s\n", ctime(), alarm.file);
    return;
  }
  err = catch(call_other(ob, alarm.func, alarm));
  if(err) {
    log_file("system/alarm", "[%s] Error executing alarm %s: %O\n", ctime(), alarm.func, err);
  }

  save_data();
}

void parse_alarm_in_file(string alarm_file) {
  string *lines, line;

  if(!file_exists(alarm_file))
    return;

  lines = explode_file(alarm_file);
  foreach(line in lines) {
    string *parts;
    class Alarm alarm;

    parts = parse_line(line);
    alarm = create_alarm(parts, 0);

    if(!alarm)
      continue;

    alarms += ({ alarm });
  }
}

string *parse_line(string line) {
  int i, len = strlen(line);
  int in_quote = 0;
  string arg = "";
  string *args = ({});

  for(i = 0; i < len; i++) {
    if(line[i] == '"') { // Toggle in_quote status on quote character
      in_quote = !in_quote;
      if(in_quote)
        arg += "\"" ; // Add quote to argument for later parsing as string
      if(!in_quote && arg != "") { // End of quoted argument
        args += ({ arg + "\"" });
        arg = "";
      }
    } else if(line[i] == ' ' && !in_quote) { // Argument separator outside quotes
      if(arg != "") {
        args += ({ arg });
        arg = "";
      }
    } else {
      arg += line[i..i] ; // Build argument character by character
    }
  }
  if(arg != "") // Add last argument if exists
    args += ({ arg });

  return args;
}

class Alarm create_alarm(string *parts, int silent) {
  class Alarm alarm;
  string type, pattern, master, file, func, arg_line, *args;
  mixed err;
  object ob;

  err = catch {
    if(sizeof(parts) >= 5) {
      type = parts[0];
      pattern = parts[1];
      master = parts[2];
      file = parts[3];
      func = parts[4];
      if(sizeof(parts) >= 6) {
        args = parts[5..];
        args = map(args, (: stringp($1) ? from_string($1) : $1 :));
      }
    } else
      return null;
  };
  if(err) {
    log_file("system/alarm", "[%s] Error in create_alarm: %O",
      ctime(), err);
    return null;
  }

  alarm = new(class Alarm);
  alarm.type = type;
  alarm.pattern = pattern;
  alarm.file = file;
  alarm.func = func;
  alarm.args = args;
  alarm.master = master == "true" ? 1 : 0;
  alarm.id = sprintf("%s.%d", generate_uuid(), time());

  err = catch(validate_alarm(alarm, silent));
  if(err)
    return null;

  return alarm;
}

int calculate_alarm_time(class Alarm alarm, int next) {
  int current_time = time();
  int alarm_time = -1;
  string fmt, alarm_time_str, weekday, timeOfDay, alarm_date_time, current_year_str;
  int year, month, next_year, wday, alarm_wday, days_diff, hours, minutes;
  mixed err;

  err = catch {
    switch(alarm.type) {
      case "H": {
        // Current time components
        int current_hour = to_int(strftime("%H", current_time)) ; // Current hour
        int current_minute = to_int(strftime("%M", current_time)) ; // Current minute
        int alarm_minute = to_int(alarm.pattern) ; // Alarm's minute

        // Construct the next alarm time
        string next_alarm_str;
        if(alarm_minute > current_minute || (alarm_minute == current_minute && next == 0)) {
          // If the alarm minute is in the future of the current hour, or exactly now and next == 0
          next_alarm_str = strftime("%Y-%m-%d ", current_time) + sprintf("%02d:%02d", current_hour, alarm_minute);
        } else {
          // If the alarm minute has passed in the current hour, move to the next hour
          int next_hour_time = current_time + 3600 ; // Add one hour in seconds
          next_alarm_str = strftime("%Y-%m-%d ", next_hour_time) + sprintf("%02d:%02d", (current_hour + 1) % 24, alarm_minute);
        }

        alarm_time = strptime("%Y-%m-%d %H:%M", next_alarm_str);
        break;
      }
      case "D": {
        // Daily alarms should run at the specified hour and minute every day
        alarm_time_str = strftime("%Y-%m-%d ", current_time) + alarm.pattern ; // Combine current date with alarm's time
        alarm_time = strptime("%Y-%m-%d %H:%M", alarm_time_str);
        if(alarm_time <= current_time && next == 1) {
          alarm_time += 86400 ; // Add one day in seconds for the next day's same time
        }
        break;
      }

      case "W": {
        // Weekly alarms run on a specific day of the week at a specified time
        string next_day_str;
        int day_diff;
        int current_wday = to_int(strftime("%w", current_time)) ; // Day of the week, Sunday as 0
        sscanf(alarm.pattern, "%d@%d:%d", alarm_wday, hours, minutes) ; // Extract target day and time
        day_diff = (alarm_wday - current_wday + 7) % 7;
        if(day_diff == 0 && next == 1) // If today is the target day but we need the next occurrence
          day_diff = 7;

        alarm_time = current_time + (day_diff * 86400) ; // Move to the correct day
        // Recalculate the alarm time to get the exact timestamp
        next_day_str = strftime("%Y-%m-%d ", alarm_time) + sprintf("%02d:%02d", hours, minutes);
        alarm_time = strptime("%Y-%m-%d %H:%M", next_day_str);
        break;
      }

      case "M": {
        // Pre-declared variables reused for clarity
        int current_day, current_hour, current_minute;

        // Extract day, hour, and minute from the alarm pattern
        sscanf(alarm.pattern, "%d@%d:%d", alarm_wday, hours, minutes);

        // Ensure 'year' and 'month' reflect the current time
        year = to_int(strftime("%Y", current_time)) ; // Already declared
        month = to_int(strftime("%m", current_time)) ; // Already declared
        current_day = to_int(strftime("%d", current_time));
        current_hour = to_int(strftime("%H", current_time));
        current_minute = to_int(strftime("%M", current_time));

        // Construct the alarm time string for this month
        alarm_date_time = sprintf("%04d-%02d-%02d %02d:%02d", year, month, alarm_wday, hours, minutes);
        alarm_time = strptime("%Y-%m-%d %H:%M", alarm_date_time);

        if(alarm_time <= current_time && next == 1) {
          // Logic to adjust the month for the next occurrence
          month += 1;
          if(month > 12) {
            month = 1 ; // Reset month to January
            year += 1 ; // Increment the year
          }

          // Recalculate the alarm time for the next month
          alarm_date_time = sprintf("%04d-%02d-%02d %02d:%02d", year, month, alarm_wday, hours, minutes);
          alarm_time = strptime("%Y-%m-%d %H:%M", alarm_date_time);
        }
        break;
      }

      case "Y": {
        // Yearly alarms run on a specific month, day, and time each year
        string next_alarm_str = strftime("%Y-", current_time) + alarm.pattern ; // Current year with alarm's month, day, and time
        alarm_time = strptime("%Y-%m-%d@%H:%M", next_alarm_str);
        if(alarm_time <= current_time && next == 1) {
          // Construct for the next year if the time for this year has passed
          next_alarm_str = sprintf("%d-", to_int(strftime("%Y", current_time)) + 1) + alarm.pattern;
          alarm_time = strptime("%Y-%m-%d@%H:%M", next_alarm_str);
        }
        break;
      }

      case "O": {
        // One-time alarms with a specific date and time
        alarm_time = strptime("%y-%m-%d@%H:%M", alarm.pattern);
        break;
      }

      default:
        return -1 ; // For unhandled types
    }
  };

  if(err) {
    log_file("system/alarm", "[%s] Error in calculate_alarm_time: %O",
      ctime(), err);
    return -1;
  }

  return alarm_time;
}

class Alarm find_alarm_by_id(string id) {
  int i;

  for(i = 0; i < sizeof(alarms); i++) {
    if(alarms[i].id == id)
      return alarms[i];
  }

  return null;
}

class Alarm* query_alarms() {
  return alarms;
}

private int next_minute_start() {
  int current_time = time() ; // Current UNIX timestamp
  int seconds_to_next_minute = 60 - (current_time % 60) ; // Seconds remaining to next minute
  int next_minute_time = current_time + seconds_to_next_minute ; // Timestamp of the next minute start

  return next_minute_time;
}

/**
 * @function execute_boot_alarms
 * @description Called from the master object when the mud boots up.
 */
void execute_boot_alarms(mixed arg...) {
  class Alarm *boot_alarms, boot_alarm;

  if(previous_object() != signal_d())
    return;

  boot_alarms = filter(alarms, (: $1.type == "B" :));
  foreach(boot_alarm in boot_alarms) {
    int seconds;

    seconds = to_int(boot_alarm.pattern);

    if(nullp(seconds))
      continue;

    if(seconds < 0)
      continue;

    call_out_walltime("execute_alarm", seconds, boot_alarm);
  }
}

int validate_alarm(class Alarm alarm, int silent) {
  mixed err;
  object ob;

  if(!cfile_exists(alarm.file)) {
    log_file("system/alarm", "[%s] File %s does not exist\n%O", ctime(), alarm.file, alarm);
    if(!silent)
      throw("File does not exist");
    return 0;
  }

  if(err = catch(ob = load_object(alarm.file))) {
    log_file("system/alarm", "[%s] Unable to load file %s: %O\n%O", ctime(), alarm.file, err, alarm);
    if(!silent)
      throw("Unable to load file");
    return 0;
  }

  if(!function_exists(alarm.func, ob)) {
    log_file("system/alarm", "[%s] Function %s does not exist in file %s\n%O", ctime(), alarm.func, alarm.file, alarm);
    if(!silent)
      throw("Function does not exist");
    return 0;
  }

  if(alarm.type == "O") {
    int time = calculate_alarm_time(alarm, 0);

    if(time < time()) {
      log_file("system/alarm", "[%s] Time is in the past\n%O", ctime(), alarm);
      if(!silent)
        throw("Time is in the past");
      return 0;
    }
  }

  return 1;
}

int time_to_next_poll() {
  return find_call_out(cid);
}

// This function is used to test the alarm daemon
varargs void test_alarm(class Alarm alarm) {
  if(!alarm)
    return;

  log_file("alarms", sprintf("Alarm %O: %O called at %s", alarm.args..., ctime()));
}
