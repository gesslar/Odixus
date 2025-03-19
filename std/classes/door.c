/**
 * @file /std/classes/door.c
 *
 * Defines the Door class used to represent physical doors, gates, and other
 * passageways between rooms. Each door has state tracking (open/closed/locked),
 * descriptive text, and directional information.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Enhanced documentation
 */

#ifndef __DOOR_C__
#define __DOOR_C__

/**
 * Represents a door or similar barrier between two connected rooms.
 *
 * A door controls access between rooms and maintains its state. It can be
 * opened, closed, and potentially locked. Multiple doors in the same room are
 * distinguished by their IDs and direction.
 *
 * @property {string*} id - Parsed identifiers, e.g. ({"oak door", "door"})
 * @property {string} name - Display name for messages, e.g. "oak door"
 * @property {string} status - Current state: "open", "closed", or "locked"
 * @property {string} type - Physical form like "door", "gate", "portal"
 * @property {string} short - Brief room description of the door
 * @property {string} long - Detailed examination description
 * @property {string} direction - Exit direction where door is located
 *
 * @example
 * class Door door = new(class Door);
 * door->id = ({"oak door", "door"});
 * door->name = "oak door";
 * door->status = "closed";
 * door->type = "door";
 * door->direction = "north";
 * door->short = "A sturdy oak door";
 * door->long = "This heavy oak door is reinforced with iron bands.";
 */
class Door {
  string *id;
  string name;
  string status;
  string type;
  string short;
  string long;
  string direction;
}

#endif // __DOOR_C__
