/**
 * @file /adm/simul_efun/file.c
 *
 * File manipulation and querying simul-efuns. Provides secure methods for
 * reading, writing, and managing files, including logging and temporary files.
 *
 * @created 2005-04-02 - Tacitus
 * @last_modified 2024-03-11
 *
 * @history
 * 2005-07-01 - Tacitus - Updated file_owner function
 * 2005-04-02 - Tacitus - Created
 */

#include <simul_efun.h>

/**
 * Ensures a file's parent directories exist.
 *
 * Creates any missing directories in the path to the specified file.
 *
 * @param {string} file - Path to the target file
 * @returns {int|string} 1 on success, error message on failure
 * @example
 * assure_file("/log/system/startup.log");
 */
mixed assure_file(string file) {
  string *parts;
  string *path;
  string dir;
  int i;

  if(nullp(file))
    return "No file specified for assure_file().\n";

  if(file_size(file) != -1)
    return 1;

  parts = dir_file(file);
  dir = parts[0];

  if(!assure_dir(dir))
    return "Failed to create directory: " + dir + "\n";

  return file_size(dir) == -2;
}

//file_owner(string file);

//Tacitus @ LPUniversity
//02-APR-05
//Simul-Efun

//Last edited on July 1st, 2005 by Tacitus

/**
 * Determines ownership of a file based on its path.
 *
 * Analyzes path structure to identify file owner, with special handling
 * for admin directories.
 *
 * @param {string} file - Path to analyze
 * @returns {string} Owner name or "Admin/name" for admin files, 0 if not found
 * @example
 * string owner = file_owner("/home/players/bob/workroom.c");  // Returns "bob"
 */
string file_owner(string file) {
  string temp;

  if(file[0] != '/') file = "/" + file;

  if(sscanf(file, "/home/%s/%s/%*s", temp, temp) == 2)
      return temp;

  if(sscanf(file, "/adm/%s/%*s", temp) == 2)
    return "Admin/" + temp;

  return 0;
}

/**
 * Returns the last n lines of a file.
 *
 * Reads from end of file efficiently, even for large files.
 *
 * @param {string} path - File to read
 * @param {int} [line_count=25] - Number of lines to return
 * @returns {string} Requested lines with proper newline formatting
 * @errors If file path is null or invalid
 * @example
 * tell_me(tail("/log/system.log", 10));  // Shows last 10 log entries
 */
varargs string tail(string path, int line_count) {
  int chunk_size = 80 * 5; // Chunk size for each read operation
  string result = ""; // Accumulator for the result
  int total_lines = 0; // Counter for total lines found
  int file_size; // Total size of the file
  int start, end; // Variables to define the read range
  string chunk; // Variable to hold the data read in each chunk
  string *lines; // Array to hold split lines
  int start_index; // Index for trimming lines

  if(nullp(path))
    error("No file specified for tail(). [" + previous_object() + "]");

  if(nullp(line_count))
    line_count = 25; // Default to 25 lines if not specified

  // Get the total size of the file
  file_size = file_size(path);

  // Initialize reading position at the end of the file
  end = file_size;

  // Ensure we don't start reading beyond the start of the file
  if(end < 0)
    return "File does not exist or is empty.";

  while(end > 0) {
    start = end - chunk_size;

    if(start < 0)
      start = 0; // Adjust start to not go below file beginning

    // Read the chunk from the file
    chunk = read_bytes(path, start, end - start);

    if(!strlen(chunk))
      break; // Break if no data was read

    // Prepend the current chunk to the result
    result = chunk + result;

    // Count the number of newlines in the current result
    total_lines = sizeof(explode(result, "\n")) - 1;

    // If we have enough lines, break the loop
    if(total_lines >= line_count)
      break;

    end = start; // Move the end position for the next read
  }

  // Trim the result to exactly the number of lines requested
  lines = explode(result, "\n");
  start_index = (sizeof(lines) > line_count) ? sizeof(lines) - line_count : 0;
  result = implode(lines[start_index..], "\n");

  // Add a newline at the beginning if we trimmed lines
  if(start_index > 0)
    result = "\n" + result;

  // Pad with empty lines if we have fewer lines than requested
  if(sizeof(lines) < line_count)
    result = implode(allocate(line_count - sizeof(lines), ""), "\n") + result;

  return result;
}


//log_file(string file, string str)

//Tacitus @ LPUniversity
//15-APR-05
//Simul-efuns

/**
 * Writes a message to a log file.
 *
 * Routes logging through master object for security.
 *
 * @param {string} file - Log file name
 * @param {string} str - Message to log
 * @param {mixed} [arg] - Optional formatting arguments
 * @returns {int} 1 on success, 0 on failure
 * @example
 * log_file("system.log", "Server started at %s", ctime());
 */
varargs int log_file(string file, string str, mixed arg...) {
  if(!file || !str)
    return 0;

  master()->log_file(file, str, arg...);

  return 1;
}

/**
 * Reads file content into array of lines.
 *
 * Filters out comments and empty lines.
 *
 * @param {string} file - File to read
 * @returns {string*} Array of non-empty, non-comment lines
 * @example
 * string *config = explode_file("/etc/config.rc");
 */
string *explode_file(string file) {
  string old_privs;

  if(!file)
    return ({});

  old_privs = query_privs();
  set_privs(this_object(), query_privs(previous_object()));

  if(!file_exists(file)) {
    set_privs(this_object(), old_privs);
    return ({});
  }

  catch {
    string data = read_file(file);
    string *lines;

    if(!data) {
      set_privs(this_object(), old_privs);
      return ({});
    }
    lines = explode(data, "\n");
    lines = filter(lines, (: $1[0] != '#' :));
    lines = filter(lines, (: strlen(trim($1)) > 0 :));
  };

  set_privs(this_object(), old_privs);

  return lines;
}

/**
 * Writes array of lines to a file.
 *
 * @param {string} file - Target file path
 * @param {string*} lines - Lines to write
 * @param {int} [overwrite=0] - Whether to overwrite existing content
 * @example
 * implode_file("/save/names.txt", ({ "Bob", "Alice" }), 1);
 */
varargs void implode_file(string file, string *lines, int overwrite) {
  string old_privs;

  if(!file) return;
  if(!lines) return;
  if(!sizeof(lines)) return;

  overwrite = !!overwrite;
  old_privs = query_privs();
  set_privs(this_object(), query_privs(previous_object()));

  catch(write_file(file, implode(lines, "\n") + "\n", overwrite));

  set_privs(this_object(), old_privs);
}

/**
 * Gets filename portion of an object's path.
 *
 * @param {object} ob - Object to query, defaults to previous_object()
 * @returns {string} Base filename without path
 * @errors If no valid object available
 * @example
 * string name = query_file_name(this_object());  // Returns "room.c"
 */
string query_file_name(object ob) {
  string file, *parts;

  if(!objectp(ob))
    ob = previous_object();

  if(!objectp(ob))
    error("Bad argument 1 to query_file_name().\n");

  file = base_name(ob);
  parts = explode(file, "/");
  file = parts[<1];

  return file;
}

/**
 * Generates a unique temporary file path.
 *
 * Creates path based on object name or provided string with timestamp.
 *
 * @param {object|string} [arg] - Base for temp filename, defaults to previous_object()
 * @returns {string} Full path to temporary file
 * @errors If argument is invalid type
 * @example
 * string tmp = temp_file("backup");  // Returns "/tmp/backup.1234567890"
 */
varargs string temp_file(mixed arg) {
  string file;
  string *matches;

  if(nullp(arg))
    arg = previous_object();

  if(stringp(arg))
    file = arg;
  else if(objectp(arg))
    file = query_privs(arg);
  else
    error("Bad argument 1 to temp_file().\n");

  matches = pcre_extract(file, "^\\[([a-zA-Z0-9_-]+)\\]$|^([a-zA-Z0-9_-]+)$");
  if(sizeof(matches) < 1)
      error("Bad argument 1 to temp_file().\n");

  if(sizeof(matches) == 1)
    file = sprintf("%s.%d", matches[0], time_ns());
  else
    file = sprintf("%s.%d", matches[1], time_ns());

  return sprintf("%s%s", tmp_dir(), file);
}

/**
 * Splits path into directory and filename components.
 *
 * @param {string|object} path - Path or object to analyze
 * @returns {({ string, string })} Tuple of ({ directory, filename })
 * @example
 * string *parts = dir_file("/d/town/square.c");
 * // Returns ({ "/d/town/", "square.c" })
 */
string *dir_file(mixed path) {
  string *matches, dir, file;

  if(nullp(path))
    path = previous_object();

  if(objectp(path))
    path = base_name(path);

  if(!stringp(path) || !strlen(path))
    error("Bad argument 1 to dir_file().\n");

  path = prepend(path, "/");
  matches = pcre_extract(path, "^(.*/)([^/]+)$");
  if(sizeof(matches) < 2) {
    return ({});
  }

  dir = matches[0];
  file = matches[1];

  return ({ dir, file });
}

/**
 * Verifies directory and optionally file existence.
 *
 * Traverses path components to validate directory structure and optionally
 * checks if the final component exists as a file.
 *
 * @param {string} path - Path to verify
 * @param {int} [file_too=0] - Whether to check file existence
 * @returns {({ string, string })} Tuple of ({ directory_path, filename }),
 *                                 or null if invalid
 * @example
 * string *parts = valid_dir_file("/d/town/square.c", 1);
 * // Returns ({ "/d/town/", "square.c" }) if path exists
 */
varargs string *valid_dir_file(string path, int file_too) {
  string *parts;
  int sz, i;
  string dir = "/";

  if(nullp(path))
    return null;

  if(directory_exists(path)) {
    if(file_too)
      return null;

    return ({ prepend(append(path, "/"), "/"), "" });
  }

  parts = explode(path, "/");
  while(sizeof(parts) > 1) {
    if(directory_exists(dir + parts[0]))
      dir += append(parts[0], "/");
    else
      break;

    parts = parts[1..];
  }

  if(file_too) {
    if(file_exists(dir + parts[0]))
      return ({ dir, parts[0] });
    else
      return null;
  }
  return ({ dir, parts[0] });
}

/**
 * Creates or updates a file's timestamp.
 *
 * Creates parent directories if needed.
 *
 * @param {string} file - File to touch
 * @returns {int} 1 if successful, 0 otherwise
 * @example
 * touch("/var/run/daemon.pid");
 */
int touch(string file) {
  string old_privs;

  if(!file)
    return 0;

  old_privs = query_privs();
  set_privs(this_object(), query_privs(previous_object()));

  catch {
    assure_file(file);
    write_file(file, "");
  };

  set_privs(this_object(), old_privs);

  return file_size(file) != -1;
}
