/**
 * @file /std/container/storage_object.c
 * @description Object for permanent or temporary storage, like shops or
 *              armouries.
 *
 * @created 2024-08-01 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-08-01 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

#include <classes.h>

inherit STD_ITEM;
inherit STD_CONTAINER;

inherit CLASS_STORAGE;

public nomask void save_contents();
private nomask void restore_contents();

private nomask class StorageOptions storage_options;
private nosave string link;

/**
 * Initialises the storage object with default settings.
 *
 * Sets the container to ignore capacity and weight limitations.
 */
void mudlib_setup() {
  set_ignore_capacity(1);
  set_ignore_mass(1);
  // set_debug_clean(1);
}

/**
 * Configures storage behaviours based on provided options.
 *
 * Sets up the storage identification, file paths, and optionally
 * restores contents from a saved file.
 *
 * @param {class StorageOptions} storage - Configuration for this storage
 * @errors If neither storage_id nor storage_org is specified
 * @errors If storage_id does not resolve to a string
 * @errors If private storage is missing required directory info
 */
void set_storage_options(class StorageOptions storage) {
    mixed id;

    storage_options = storage;

    id = storage_options.storage_id;
    if(id) {
        if(valid_function(id))
            id = (*id)();
    } else if(storage_options.storage_org) {
        id = storage_options.storage_org;
    } else {
        error("Either storage_id or storage_org must be specified in storage options.");
    }

    if(!stringp(id))
        error("storage_id must resolve to a string.");

    storage_options.storage_id = id;

    if(storage_options.restore_on_load) {
        if(storage_options.storage_type == "private") {
            // we must have an storage_directory. if we don't, then we can use
            // the storage_org

            if(!storage_options.storage_directory)
                if(!storage_options.storage_org)
                    error("For private storage, storage_directory is only optional if storage_org is specified.");
                else
                    storage_options.storage_directory = storage_options.storage_org;
        }

        restore_contents();
    }
}

/**
 * Returns the current storage options.
 *
 * @returns {class StorageOptions} The current storage configuration
 */
class StorageOptions query_storage_options() {
    return storage_options;
}

/**
 * Saves the contents of the storage to a persistent file.
 *
 * If the storage is empty and a file exists, it will be removed.
 * Only saves if restore_on_load is enabled and a storage_id is set.
 */
public nomask void save_contents() {
    if(classp(storage_options)) {
        if(storage_options.restore_on_load &&
           storage_options.storage_id) {
            string storage_dir = mud_config("STORAGE_DATA_DIR");
            string dest;
            string data;

            if(storage_options.storage_directory) {
                storage_dir = resolve_path(storage_dir, storage_options.storage_directory);
                storage_dir = append(storage_dir, "/");
            }

            dest = sprintf("%s%s",
                storage_dir,
                storage_options.storage_id
            );

            assure_file(dest);

            if(sizeof(all_inventory())) {
                data = save_to_string(1);
                write_file(dest, data, 1);
            } else {
                if(file_exists(dest))
                    rm(dest);
            }
        }
    }
}

/**
 * Restores storage contents from a saved file.
 *
 * Only restores if restore_on_load is enabled and a storage_id is set.
 * Sets up the storage directory for future save operations.
 */
public nomask void restore_contents() {
    if(classp(storage_options)) {
        if(storage_options.restore_on_load &&
           storage_options.storage_id) {
            string storage_dir = mud_config("STORAGE_DATA_DIR");
            string dest;
            string data;

            if(storage_options.storage_directory) {
                storage_dir = resolve_path(storage_dir, storage_options.storage_directory);
                storage_dir = append(storage_dir, "/");
            }
            storage_options.storage_directory = storage_dir;

            dest = sprintf("%s%s",
                storage_dir,
                storage_options.storage_id
            );

            if(file_exists(dest)) {
                data = read_file(dest);
                if(sizeof(data))
                    load_from_string(data, 1);

                save_contents();
            }
        }
    }
}

/**
 * Sets whether the storage object should be cleaned up when empty.
 *
 * @param {int} clean - 1 to enable cleanup when empty, 0 to disable
 */
void set_clean_on_empty(int clean) {
    storage_options.clean_on_empty = clean;
}

/**
 * Checks if the storage object will be cleaned up when empty.
 *
 * @returns {int} 1 if cleanup is enabled, 0 if disabled
 */
int query_clean_on_empty() {
    return storage_options.clean_on_empty;
}

/**
 * Sets whether storage contents should be restored from file on load.
 *
 * @param {int} restore - 1 to enable restore on load, 0 to disable
 */
void set_restore_on_load(int restore) {
    storage_options.restore_on_load = restore;
}

/**
 * Checks if storage contents are restored from file on load.
 *
 * @returns {int} 1 if restore is enabled, 0 if disabled
 */
int query_restore_on_load() {
    return storage_options.restore_on_load;
}

/**
 * Sets the storage identifier for file operations.
 *
 * @param {string} id - The unique identifier for this storage
 */
void set_storage_id(string id) {
    storage_options.storage_id = id;
}

/**
 * Returns the storage identifier.
 *
 * @returns {string} The storage identifier
 */
string query_storage_id() {
    return storage_options.storage_id;
}

/**
 * Sets the directory where storage files are saved.
 *
 * @param {string} dir - The directory path
 */
void set_storage_directory(string dir) {
    storage_options.storage_directory = dir;
}

/**
 * Returns the directory where storage files are saved.
 *
 * @returns {string} The directory path
 */
string query_storage_directory() {
    return storage_options.storage_directory;
}

/**
 * Determines if the object should be cleaned up during garbage collection.
 *
 * Returns true if the storage is empty and clean_on_empty is enabled.
 *
 * @returns {int} 1 if the object should be cleaned up, 0 if not
 */
int request_clean_up() {
    if(sizeof(all_inventory()) == 0 && classp(storage_options) &&
       storage_options.clean_on_empty)
        return 1;

    return 0;
}

/**
 * Checks if a given ID matches this storage.
 *
 * In addition to normal ID checks, also returns true if the ID matches
 * the storage_id.
 *
 * @param {string} id - The ID to check
 * @returns {int} 1 if ID matches, 0 if not
 */
int id(string id) {
    if(classp(storage_options)) {
        if(storage_options.storage_id)
            if(storage_options.storage_id == id)
                return 1;
    }

    return ::id(id);
}

/**
 * Sets the link to the associated storage room.
 *
 * @param {string} link - The filename of the linked storage room
 * @returns {string} The link that was set
 */
string set_link(string link) {
    return link;
}

/**
 * Returns the link to the associated storage room.
 *
 * @returns {string} The filename of the linked storage room
 */
string query_link() {
    return link;
}

/**
 * Identifies this object as a storage object.
 *
 * Used for type checking.
 *
 * @returns {int} Always returns 1
 */
int is_storage_object() { return 1 ; }
