/**
 * @file /adm/daemons/swap.c
 *
 * Temporary storage daemon that provides a safe way to persist data across
 * object destruction and recreation. Acts as a holding space for data that
 * needs to survive an object's temporary absence.
 *
 * Common use cases:
 * - Preserving state during object reloads
 * - Maintaining data during intentional destruction/recreation cycles
 * - Temporary storage during system transitions
 *
 * The daemon uses a simple label-based store/retrieve pattern:
 * 1. Object stores data with swap_in(label, data)
 * 2. Object can be safely destructed
 * 3. New instance retrieves data with swap_out(label)
 * 4. Data is automatically removed after retrieval
 *
 * @created 2024-02-20 - Gesslar
 * @last_modified 2024-02-20 - Gesslar
 *
 * @history
 * 2024-02-20 - Gesslar - Created
 */

inherit STD_DAEMON;

// Forward declarations
void swap_in(string label, mixed value);
mixed swap_out(string label);

private nosave mapping data = ([]);

/**
 * Stores data in the swap daemon.
 *
 * Data is held until explicitly retrieved via swap_out(). This allows
 * objects to safely store state before destruction, knowing it can be
 * retrieved when they are recreated.
 *
 * @param {string} label - Unique identifier for the stored data
 * @param {mixed} value - Data to be stored
 */
void swap_in(string label, mixed value) {
  data[label] = value;
}

/**
 * Retrieves and removes data from the swap daemon.
 *
 * Once data is retrieved, it is immediately removed from storage.
 * This ensures data is only accessed once and maintains clean state.
 *
 * @param {string} label - Unique identifier for the stored data
 * @returns {mixed} The stored data, or 0 if no data found
 */
mixed swap_out(string label) {
  mixed value = data[label];

  map_delete(data, label);

  return value;
}

/**
 * Clean up handler that prevents daemon destruction while holding data.
 *
 * @param {int} refs - Number of references to this object
 * @returns {int} CLEAN_LATER if holding data, otherwise standard clean up result
 */
int clean_up(int refs) {
  if(sizeof(data) > 0)
    return CLEAN_LATER;

  return ::clean_up(refs);
}
