/**
 * @file /std/classes/storage.c
 *
 * Class definition for StorageOptions used in storage room implementations.
 *
 * @created 2025-03-16 - GitHub Copilot
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

#ifndef __CLASS_STORAGE_OPTIONS__
#define __CLASS_STORAGE_OPTIONS__

/**
 * Configures behavior of storage objects and rooms.
 *
 * @property {string} storage_type - Type of storage ("public", "private", or custom)
 * @property {mixed} storage_id - ID for private storage (string or function)
 * @property {string} storage_org - Organization name for storage directory
 * @property {string} storage_directory - Directory path for storage
 * @property {int} clean_on_empty - Whether to clean storage when empty
 * @property {int} restore_on_load - Whether to restore storage on load
 */
class StorageOptions {
  string storage_type;
  mixed storage_id;
  string storage_org;
  string storage_directory;
  int clean_on_empty;
  int restore_on_load;
}

#endif // __CLASS_STORAGE_OPTIONS__
