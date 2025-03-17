/**
 * @file /std/room/storage.c
 * @description This is a room that enables players to access storage. Inherit
 *              from this class to create a storage room.
 *
 * @created 2024-08-12 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-08-12 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

#include <classes.h>
#include "/std/object/include/command.h"
#include "/std/object/include/init.h"
#include "/std/object/include/object.h"

inherit CLASS_STORAGE;

private nomask void store();
private nomask void destructing();

private class StorageOptions storage_options;

/**
 * Initialises a storage room with commands and cleanup handler.
 *
 * This adds the 'list', 'store', and 'take' commands to the room
 * and sets up object cleanup on destruction.
 */
void init_storage_room() {
  add_command("list", "cmd_list");
  add_command("store", "cmd_store");
  add_command("take", "cmd_take");

  add_destruct((: destructing :));
}

/**
 * Creates or retrieves the storage object for this room.
 *
 * Creates or finds the appropriate storage object based on configuration.
 * For "public" storage, uses /storage/[organization].
 * For "private" storage, uses /storage/[organization]/[id].
 * For custom storage, loads /obj/storage/[organization].
 *
 * @returns {STD_STORAGE_OBJECT} The storage object
 * @errors If storage organization is invalid
 * @errors If storage ID for private storage is invalid
 * @errors If custom storage file is not found
 */
object store() {
  object store;
  string org, file;

  org = storage_options.storage_org;
  if(!stringp(org))
    error("Invalid storage organization specified");

  switch(storage_options.storage_type) {
    case "public":
      file = sprintf("/storage/%s", org);
      break;
    case "private": {
      mixed id = storage_options.storage_id;

      if(valid_function(id))
        id = (*id)();

      if(!stringp(id))
        error("Invalid storage ID specified for private storage");

      file = sprintf("/storage/%s/%s", org, id);
      break;
    }
    default:
      // Default to custom storage
      file = sprintf("/obj/storage/%s", org);

      if(file_size(file + ".c") < 0) {
        error("Custom storage file not found: " + file + ".c");
      }

      store = new(file);
      store->set_storage_options(storage_options);
      return store;
  }

  if(store = find_object(file))
    return store;

  store = load_object(file);
  store->set_storage_options(storage_options);
  store->set_link(file_name());

  return store;
}

/**
 * Getter for the storage object.
 *
 * @returns {object} The storage object
 */
object get_storage_object() {
  return store();
}

/**
 * Sets storage options for this room.
 *
 * @param {class StorageOptions} options - The storage configuration options
 */
void set_storage_options(class StorageOptions options) {
  storage_options = options;
}

/**
 * Returns the current storage options.
 *
 * @returns {class StorageOptions} The storage configuration options
 */
class StorageOptions query_storage_options() {
  return storage_options;
}

/**
 * Command to list items in storage.
 *
 * @param {object} tp - The player issuing the command
 * @param {string} arg - Command arguments
 * @returns {string*} Array of strings describing the storage contents
 */
mixed cmd_list(object tp, string arg) {
  string *list = ({});
  object ob, next;
  object store = store();

  ob = first_inventory(store);

  while(ob) {
    next = next_inventory(ob);
    list += ({ get_short(ob) });
    ob = next;
  }

  if(sizeof(list) > 0)
    list = ({ "You see the following items in storage", "" }) + list;
  else
    list = ({ "You see no items in storage." });

  return list;
}

/**
 * Command to store items from the player's inventory into storage.
 *
 * Handles "store all" and "store all [item]" syntax for batch storing.
 *
 * @param {object} tp - The player issuing the command
 * @param {string} arg - Command arguments
 * @returns {string} Result message
 */
mixed cmd_store(object tp, string arg) {
  object ob, *obs;
  string out = "";
  int result;
  object store = store();

  if(!arg)
    return "Usage: store <item|all|all <item>>";

  if(arg == "all")
    obs = find_targets(tp, null, tp);
  else if(sscanf(arg, "all %s", arg))
    obs = find_targets(tp, arg, tp);
  else
    obs = ({ find_target(tp, arg, tp) });

  obs -= ({ 0 });

  if(!sizeof(obs))
    return "You don't possess any such thing to store.";

  foreach(ob in obs) {
    if(result = ob->move(store))
      out += get_short(ob) + " could not be stored.\n";
    else
      out += "You store " + get_short(ob) + ".\n";
  }

  if(strlen(out)) {
    if(storage_options.restore_on_load)
      store->save_contents();

      return out;
  } else {
    return "You were unable to store anything.";
  }
}

/**
 * Command to take items from storage into player's inventory.
 *
 * Handles "take all" and "take all [item]" syntax for batch retrieval.
 *
 * @param {object} tp - The player issuing the command
 * @param {string} arg - Command arguments
 * @returns {string} Result message
 */
mixed cmd_take(object tp, string arg) {
  object ob, *obs;
  string out = "";
  int result;
  object store = store();

  if(!arg)
    return "Usage: take <item|all|all <item>>";

  if(arg == "all")
    obs = all_inventory(store);
  else if(sscanf(arg, "all %s", arg))
    obs = filter(all_inventory(store), (: $1->id($2) :), arg);
  else
    obs = ({ present(arg, store) });

  obs -= ({ 0 });

  if(!sizeof(obs))
    return "There is no such item in storage.";

  foreach(ob in obs) {
    if((result = ob->move(tp)))
      out += get_short(ob) + " could not be taken.\n";
    else
      out += "You take " + get_short(ob) + " from storage.\n";
  }

  if(strlen(out)) {
    if(storage_options.restore_on_load)
      store->save_contents();

    return out;
  } else {
    return "You were unable to take anything from storage.";
  }
}

/**
 * Cleanup function called when the room is being destroyed.
 *
 * Finds and removes any storage objects linked to this room.
 */
void destructing() {
  object *obs = clones(STD_STORAGE_OBJECT);

  obs = filter(obs, (: $1->query_link() == file_name() :));

  filter(obs, (: $1->remove() :));
}
