/**
 * @file /std/object/cooldown.c
 * @description Manages time-limited actions via cooldown timers.
 *
 * @created 2024-09-15 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-09-15 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

#include <cooldown.h>

private mapping _cooldowns = ([]);

/**
 * Removes expired cooldowns from the cooldown mapping.
 *
 * Should be called periodically to clean up old entries.
 */
void cooldown() {
  _cooldowns = _cooldowns || ([]);
  foreach(string id, int time in _cooldowns)
    if(time < time())
      map_delete(_cooldowns, id);
}

/**
 * Returns a copy of all active cooldowns.
 *
 * @returns {mapping} Copy of the cooldowns mapping (ID to expiry time)
 */
mapping query_cooldowns() {
  return copy(_cooldowns);
}

/**
 * Sets a cooldown to expire at a specific time.
 *
 * @param {string} id - The cooldown identifier
 * @param {int} cooldown - The absolute time when cooldown expires
 */
void set_cooldown(string id, int cooldown) {
  _cooldowns[id] = cooldown;
}

/**
 * Adds a new cooldown only if no cooldown exists for this ID.
 *
 * @param {string} id - The cooldown identifier
 * @param {int} cooldown - Duration in seconds
 * @returns {int} The expiry time of the new cooldown, or null if not added
 */
int add_cooldown(string id, int cooldown) {
  int time = query_cooldown(id);

  if(time > 0)
    return null;

  _cooldowns[id] = time() + cooldown;

  return query_cooldown(id);
}

/**
 * Removes a cooldown.
 *
 * @param {string} id - The cooldown identifier to remove
 * @returns {int} 1 on success, null if cooldown doesn't exist
 */
int remove_cooldown(string id) {
  if(!nullp(query_cooldown(id)))
    return null;

  map_delete(_cooldowns, id);

  return 1;
}

/**
 * Returns the expiry time of a cooldown.
 *
 * @param {string} id - The cooldown identifier
 * @returns {int} The absolute time when cooldown expires
 */
int query_cooldown(string id) {
  _cooldowns = _cooldowns || ([]);

  return _cooldowns[id];
}

/**
 * Calculates the time remaining on a cooldown.
 *
 * @param {string} id - The cooldown identifier
 * @returns {int} Remaining time in seconds, or 0 if expired/non-existent
 */
int query_cooldown_remaining(string id) {
  int time = query_cooldown(id);

  if(time < 0)
    return 0;

  return time - time();
}

/**
 * Adjusts an existing cooldown's expiry time.
 *
 * @param {string} id - The cooldown identifier
 * @param {int} amount - The amount to adjust by (positive or negative)
 * @returns {int} The new expiry time, or null if cooldown doesn't exist
 */
int adjust_cooldown(string id, int amount) {
  int time = query_cooldown(id);

  if(time < 0)
    return null;

  _cooldowns[id] = time + amount;

  return query_cooldown(id);
}

/**
 * Removes all cooldowns.
 *
 * @returns {int} Always returns 1
 */
int wipe_cooldowns() {
  _cooldowns = ([]);

  return 1;
}
