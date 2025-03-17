/**
 * @file /std/object/stats.c
 * @description Tracks creation time, age, and creator information for objects.
 *
 * @created 2024-02-04 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-02-04 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

private nosave int _create_time = time();
private nosave object _cloned_by = previous_object();
private nosave string _cloned_by_string = file_name(previous_object());

/**
 * Returns the timestamp when this object was created.
 *
 * @returns {int} Unix timestamp of creation time
 */
int query_object_create_time() { return _create_time; }

/**
 * Returns the age of this object in seconds.
 *
 * @returns {int} The object's age in seconds
 */
int query_object_age() { return time() - _create_time; }

/**
 * Returns the object that created this object.
 *
 * @returns {object} The creator object
 */
object query_cloned_by() { return _cloned_by; }

/**
 * Returns the file name of the object that created this object.
 *
 * @returns {string} The creator's file name
 */
string query_cloned_by_string() { return _cloned_by_string; }
