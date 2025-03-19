/**
 * @file /adm/daemons/gmcp.c
 *
 * GMCP (Generic Mud Communication Protocol) daemon responsible for handling
 * all GMCP-related communications between the MUD and clients.
 *
 * @created 2024-02-22 - Gesslar
 * @last_modified 2024-02-22 - Gesslar
 *
 * @history
 * 2024-02-22 - Gesslar - Created
 */

#include <classes.h>
#include <gmcp_defines.h>

inherit STD_DAEMON;
inherit CLASS_GMCP;

class ClassGMCP convert_message(string message);
varargs void send_gmcp(object body, string gmcp_package, mixed arg);
varargs void broadcast_gmcp(mixed audience, string gmcp_package, mixed arg);
void init_gmcp(object who);

/**
 * Converts a GMCP message string into a structured ClassGMCP object.
 *
 * This function parses a GMCP message string into its component parts and creates
 * a ClassGMCP object containing the package, module, submodule (if any), and payload.
 * The message format is expected to be "Package.Module.Submodule payload" where
 * the payload is optional JSON data.
 *
 * @param {string} message - The GMCP message to convert
 * @returns {class ClassGMCP} The converted GMCP message structure, or null if invalid
 * @errors If JSON decoding of the payload fails
 */
class ClassGMCP convert_message(string message) {
  class ClassGMCP gmcp = new(class ClassGMCP);
  string *parts;
  string package_info, message_info;
  int pos;
  int sz;

  message = trim(message);

  pos = strsrch(message, " ");
  if(pos == -1) {
    package_info = message;
    message_info = null;
  } else {
    package_info = message[0..pos-1];
    message_info = message[pos+1..];
  }

  gmcp.name = package_info;

  parts = explode(package_info, ".");
  sz = sizeof(parts);
  if(sz >= 1) {
    gmcp.package = parts[0];
    if(sz >= 2) {
      if(sz == 2)
        gmcp.module = parts[1];
      else if(sz == 3) {
        gmcp.module = parts[1];
        gmcp.submodule = parts[2];
      }
    }
  }

  if(!gmcp.package)
    return null;
  if(!gmcp.module)
    return null;

  if(message_info == null)
    gmcp.payload = null;
  else {
    mixed err;
    err = catch {
      gmcp.payload = json_decode(message_info);
    };
    if(err)
      debug("Error decoding JSON: %O", chop(err, "\n", -1));
  }

  return gmcp;
}

/**
 * Sends a GMCP message to a specific player or object.
 *
 * This function handles the actual sending of GMCP messages to individual targets.
 * It validates the target, checks GMCP support, loads the appropriate module,
 * and dispatches the message.
 *
 * @param {object} body - The target object to receive the GMCP message
 * @param {string} gmcp_package - The GMCP package identifier
 * @param {mixed} [arg] - Optional payload data for the GMCP message
 * @errors If the GMCP module cannot be found or loaded
 */
varargs void send_gmcp(object body, string gmcp_package, mixed arg) {
  mixed *packet;
  class ClassGMCP gmcp;
  string gmcp_module;
  mixed err;
  object ob;
  string base;

  if(!get_config(__RC_ENABLE_GMCP__))
    return;

  if(!body || !gmcp_package)
    return;

  base = base_name(body);
  if(base == LOGIN_OB) {
    if(!has_gmcp(body))
      return;
  } else if(userp(body) || ghostp(body)) {
    if(!body->gmcp_enabled())
      return;
  } else
    return;

  gmcp = convert_message(gmcp_package);
  if(gmcp == null || (gmcp.package == null && gmcp.module == null))
    return;

  gmcp.payload = arg;

  gmcp_module = __DIR__ "modules/gmcp/" + gmcp.package + ".c";

  if(!file_exists(gmcp_module)) {
    log_file("system/gmcp", "[%s] Module %s not found [%O]",
      ctime(),
      gmcp_module,
      previous_object()->query_gmcp_module() ?
        previous_object(1) :
        previous_object()
    );
    return;
  }

  if(err = catch(ob = load_object(gmcp_module)))
    return;

  if(gmcp.submodule)
    call_other(gmcp_module, gmcp.module, body, gmcp.submodule, gmcp.payload);
  else
    call_other(gmcp_module, gmcp.module, body, gmcp.payload);
}

/**
 * Broadcasts a GMCP message to multiple recipients.
 *
 * This function handles broadcasting GMCP messages to either a single target,
 * all players in a room, or an array of targets.
 *
 * @param {object|object*} audience - Target(s) to receive the message. Can be:
 *   - A room object (broadcasts to all players in the room)
 *   - A player object (sends to just that player)
 *   - An array of objects (sends to all valid targets in array)
 * @param {string} gmcp_package - The GMCP package identifier
 * @param {mixed} [arg] - Optional payload data for the GMCP message
 */
varargs void broadcast_gmcp(mixed audience, string gmcp_package, mixed arg) {
  object *targets = ({ });

  if(!get_config(__RC_ENABLE_GMCP__) || !gmcp_package || nullp(audience))
    return;

  if(objectp(audience)) {
    if(audience->is_room()) {
      targets = present_players(audience);
      if(!sizeof(targets))
        return;
    } else if(audience->is_player())
      targets += ({ audience });
  } else if(arrayp(audience))
    targets += audience;
  else
    return;
  foreach(object target in targets)
    send_gmcp(target, gmcp_package, arg);
}

/**
 * Initializes GMCP for a player by sending initial state information.
 *
 * This function sends a series of GMCP packages to initialize the client's state,
 * including character status, vitals, room information, and inventory lists.
 *
 * @param {object} who - The player object to initialize GMCP for
 */
void init_gmcp(object who) {
  send_gmcp(who, GMCP_PKG_CHAR_STATUSVARS);
  send_gmcp(who, GMCP_PKG_CHAR_STATUS);
  send_gmcp(who, GMCP_PKG_CHAR_VITALS);
  send_gmcp(who, GMCP_PKG_ROOM_INFO);
  send_gmcp(who, GMCP_PKG_CHAR_ITEMS_LIST, GMCP_LIST_ROOM);
  send_gmcp(who, GMCP_PKG_CHAR_ITEMS_LIST, GMCP_LIST_INV);
}

/**
 * Configures the daemon to prevent automatic cleanup.
 */
void setup() {
  set_no_clean(1);
}
