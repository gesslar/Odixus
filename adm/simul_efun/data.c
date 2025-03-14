/**
 * @file /adm/simul_efun/data.c
 *
 * Simple key-value data storage system for file-based persistence.
 * Provides functions for reading, writing, and manipulating data
 * stored in files using a key-value format.
 *
 * @created Unknown
 * @last_modified 2024-03-11
 */

#include <simul_efun.h>

/**
 * Locates the index of a key within a file's contents.
 *
 * Searches through an array of lines to find one starting with the given key.
 * Used internally by the data storage functions.
 *
 * @param {string*} lines - Array of lines to search through
 * @param {string} key - Key to locate at start of line
 * @returns {int} Line index where key was found, or -1 if not found
 * @private
 */
private int data_index(string *lines, string key) {
  int i, sz;

  for(i = 0, sz = sizeof(lines); i < sz; i++) {
    if(strsrch(lines[i], key) == 0)
      return i;
  }

  return -1;
}

/**
 * Retrieves a value from a data file by key.
 *
 * Reads the specified file and returns the value(s) associated with the key.
 * Multiple values are returned as an array.
 *
 * @param {string} file - Path to the data file
 * @param {string} key - Key to look up
 * @param {mixed} [def] - Default value if key not found
 * @returns {mixed} Value(s) associated with key or default if not found
 * @example
 * // Single value
 * string name = data_value("/save/player.dat", "name", "Unknown");
 * // Multiple values
 * mixed *stats = data_value("/save/player.dat", "stats", ({10, 10, 10}));
 */
varargs mixed data_value(string file, string key, mixed def) {
  string *lines, line;
  mixed *parts;
  int i, sz;

  if(nullp(file) || nullp(key))
    return null;

  if(!file_exists(file))
    return null;

  key = append(key, "|");
  lines = explode_file(file);
  i = data_index(lines, key);
  if(i == -1)
    return def;

  line = lines[i];
  parts = explode(line, "|");
  parts = parts[1..];
  parts = map(parts, (: from_string :));
  if(sizeof(parts) == 1)
    return parts[0];

  return parts;
}

/**
 * Writes one or more values to a data file under a key.
 *
 * If the key exists, its value is updated. If not, a new entry is created.
 * Multiple values are stored as a single entry.
 *
 * @param {string} file - Path to the data file
 * @param {string} key - Key to write under
 * @param {mixed} data - Value(s) to store
 * @example
 * // Store single value
 * data_write("/save/player.dat", "name", "Bob");
 * // Store multiple values
 * data_write("/save/player.dat", "stats", 10, 12, 15);
 */
varargs void data_write(string file, string key, mixed data...) {
  string *lines, out;
  mixed *parts;
  int i;

  if(nullp(file) || nullp(key) || nullp(data))
    return;

  parts = map(data, (: stringify :));
  out = implode(parts, "|");
  key = append(key, "|");
  out = key + out;

  lines = explode_file(file);
  i = data_index(lines, key);
  if(i > -1) {
    lines[i] = out;
    implode_file(file, lines, 1);
    return;
  }

  implode_file(file, lines, 1);
}

/**
 * Removes a key-value pair from a data file.
 *
 * Deletes the entire line containing the specified key. If the file becomes
 * empty after deletion, the file itself is removed.
 *
 * @param {string} file - Path to the data file
 * @param {string} key - Key to remove
 * @returns {int} 1 if key was found and deleted, 0 otherwise
 * @example
 * if(data_del("/save/player.dat", "temporary_buff")) {
 *     write("Buff removed.\n");
 * }
 */
int data_del(string file, string key) {
  string *lines;
  int i;

  if(nullp(file) || nullp(key))
    return 0;

  lines = explode_file(file);
  i = data_index(lines, key);
  if(i == -1)
    return 0;

  lines = lines[0..i-1] + lines[i+1..];
  if(sizeof(lines) == 0)
    rm(file);
  else
    implode_file(file, lines, 1);

  return 1;
}

/**
 * Increments a numeric value in a data file.
 *
 * If the key doesn't exist, it's created with the increment value.
 * If it exists, its value is increased by the specified amount.
 *
 * Defaults to 1 if inc is not supplied.
 *
 * @param {string} file - Path to the data file
 * @param {string} key - Key of value to increment
 * @param {int} [inc=1] - Amount to increment by
 * @returns {int} New value after incrementing
 * @example
 * // Increment kill count
 * int kills = data_inc("/save/stats.dat", "monster_kills", 1);
 * printf("Total kills: %d\n", kills);
 */
varargs int data_inc(string file, string key, int inc) {
  string *lines, line;
  int i, val;

  if(nullp(file) || nullp(key))
    return null;

  if(nullp(inc))
    inc = 1;

  if(inc == 0)
    return data_value(file, key);

  lines = explode_file(file);
  i = data_index(lines, key);
  if(i == -1) {
    data_write(file, key, inc);
    return inc;
  }

  key = append(key, "|");
  line = lines[i];
  sscanf(line, key + "%d", val);
  val += inc;
  lines[i] = key + val;
  implode_file(file, lines, 1);
}
