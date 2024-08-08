/**
 * @file /adm/daemons/time.c
 * @description This is the game's time daemon. It is responsible for keeping
 * track of the game's time and date. It also provides functions for converting
 * the game's time to real time and vice versa.
 *
 * @created 2024/08/05 - Gesslar
 * @last_modified 2024/08/05 - Gesslar
 *
 * @history
 * 2024/08/05 - Gesslar - Created
 */

#include "include/time.h"

inherit STD_DAEMON ;

private nosave mapping config ;
private nosave mixed *months ;
private nosave string *seasons ;

private nosave int year_start, months_in_year, days_in_year, hour_length, hours_in_day ;

void setup() {
    int next_day ;

    config = json_decode(read_file("/adm/etc/time/time.json")) ;
    months = json_decode(read_file("/adm/etc/time/months.json")) ;
    seasons = json_decode(read_file("/adm/etc/time/seasons.json")) ;

    months_in_year = sizeof(months) ;
    days_in_year = sum(map(months, (: $1[1] :))) ;
    hour_length = config["hour_length"] ;
    hours_in_day = config["hours_per_day"] ;
    year_start = config["year_start"] ;

    next_day = last_midnight() + (hours_in_day * hour_length) ;
    call_out("change_day", next_day - time()) ;
}

void change_day() {
    int next_day ;

    emit(SIG_GAME_MIDNIGHT) ;

    next_day = last_midnight() + (hours_in_day * hour_length) ;
    call_out("change_day", next_day - time()) ;
}

private varargs int since_start(int t) {
    if(nullp(t))
        t = time() ;

    return t - config["epoch_start"] ;
}

varargs mixed *query_full_date(int t) {
    mixed *result = allocate(8);
    int elapsed, second, year, month, day, hour, minute;
    string month_name, season;

    elapsed = since_start(t);
    year = elapsed / (days_in_year * hours_in_day * hour_length);
    elapsed -= year * days_in_year * hours_in_day * hour_length;

    // Adjust year for starting year
    year += year_start;

    // Calculate month and day within the year
    for(month = 0; month < months_in_year; month++) {
        int days_in_this_month = months[month][1];
        int seconds_in_this_month = days_in_this_month * hours_in_day * hour_length;

        if(elapsed < seconds_in_this_month) {
            break;
        }

        elapsed -= seconds_in_this_month;
    }

    // Calculate day, hour, minute, second
    day = elapsed / (hours_in_day * hour_length);
    elapsed -= day * hours_in_day * hour_length;
    hour = elapsed / hour_length;
    elapsed -= hour * hour_length;
    minute = elapsed / 60;
    second = elapsed % 60;

    // Adjustments for correct indexing and naming
    month_name = months[month][0];
    season = months[month][2];

    result[0] = year;
    result[1] = month;
    result[2] = day + 1; // 1-based index for day
    result[3] = hour;
    result[4] = minute;
    result[5] = second;
    result[6] = month_name;
    result[7] = season;

    return result;
}

varargs int query_year(int t) {
    int elapsed, year;

    elapsed = since_start(t);
    year = elapsed / (days_in_year * hours_in_day * hour_length);

    return year + year_start;
}

varargs int query_month(int t) {
    int elapsed, month;

    elapsed = since_start(t);
    month = (elapsed / (hours_in_day * hour_length * (days_in_year / months_in_year))) % months_in_year;

    return month;
}

varargs int query_day(int t) {
    int elapsed, year, month, day;

    elapsed = since_start(t);
    year = elapsed / (days_in_year * hours_in_day * hour_length);
    elapsed -= year * days_in_year * hours_in_day * hour_length;
    month = (elapsed / (hours_in_day * hour_length * (days_in_year / months_in_year))) % months_in_year;
    elapsed -= month * hours_in_day * hour_length * (days_in_year / months_in_year);
    day = elapsed / (hours_in_day * hour_length);

    return day + 1; // Adjust for 0-based index
}

varargs int query_hour(int t) {
    int elapsed, year, month, day, hour;

    elapsed = since_start(t);
    year = elapsed / (days_in_year * hours_in_day * hour_length);
    elapsed -= year * days_in_year * hours_in_day * hour_length;
    month = (elapsed / (hours_in_day * hour_length * (days_in_year / months_in_year))) % months_in_year;
    elapsed -= month * hours_in_day * hour_length * (days_in_year / months_in_year);
    day = elapsed / (hours_in_day * hour_length);
    elapsed -= day * hours_in_day * hour_length;
    hour = elapsed / hour_length;

    return hour;
}

varargs int query_minute(int t) {
    int elapsed, year, month, day, hour, minute;

    elapsed = since_start(t);
    year = elapsed / (days_in_year * hours_in_day * hour_length);
    elapsed -= year * days_in_year * hours_in_day * hour_length;
    month = (elapsed / (hours_in_day * hour_length * (days_in_year / months_in_year))) % months_in_year;
    elapsed -= month * hours_in_day * hour_length * (days_in_year / months_in_year);
    day = elapsed / (hours_in_day * hour_length);
    elapsed -= day * hours_in_day * hour_length;
    hour = elapsed / hour_length;
    elapsed -= hour * hour_length;
    minute = elapsed / 60;

    return minute;
}

varargs int query_second(int t) {
    int elapsed, year, month, day, hour, minute, second;

    elapsed = since_start(t);
    year = elapsed / (days_in_year * hours_in_day * hour_length);
    elapsed -= year * days_in_year * hours_in_day * hour_length;
    month = (elapsed / (hours_in_day * hour_length * (days_in_year / months_in_year))) % months_in_year;
    elapsed -= month * hours_in_day * hour_length * (days_in_year / months_in_year);
    day = elapsed / (hours_in_day * hour_length);
    elapsed -= day * hours_in_day * hour_length;
    hour = elapsed / hour_length;
    elapsed -= hour * hour_length;
    minute = elapsed / 60;
    elapsed -= minute * 60;
    second = elapsed;

    return second;
}

varargs string query_season(int t) {
    return months[query_month(t)][2] ;
}

varargs string query_month_name(int t) {
    return months[query_month(t)][0] ;
}

varargs string query_date(int t) {
    mixed *date = query_full_date(t) ;
    return sprintf("%s %d, %d", date[6], date[2], date[0]) ;
}

varargs string query_time(int t) {
    mixed *date = query_full_date(t) ;
    return sprintf("%d:%02d:%02d", date[3], date[4], date[5]) ;
}

varargs string query_full_time(int t) {
    mixed *date = query_full_date(t) ;
    return sprintf("%s %d, %d %d:%02d:%02d", date[6], date[2], date[0], date[3], date[4], date[5]) ;
}

int last_midnight() {
    int elapsed, year, current_year_start, year_offset;
    int elapsed_in_year, complete_days, current_day_start;
    int last_midnight_unix_timestamp;

    // Elapsed real time in seconds since epoch start
    elapsed = since_start();

    // Calculate the number of full in-game years that have passed
    year = query_year(); // Current in-game year
    year_offset = year - year_start;
    current_year_start = year_offset * (days_in_year * hours_in_day * hour_length);

    // Time elapsed within the current year
    elapsed_in_year = elapsed - current_year_start;

    // Calculate the number of complete days passed in the current year
    complete_days = elapsed_in_year / (hours_in_day * hour_length);

    // Calculate the start of the current day in real-time seconds since epoch start
    current_day_start = current_year_start + complete_days * (hours_in_day * hour_length);

    // Calculate the Unix timestamp for the last in-game midnight
    last_midnight_unix_timestamp = config["epoch_start"] + current_day_start;

    return last_midnight_unix_timestamp;
}
