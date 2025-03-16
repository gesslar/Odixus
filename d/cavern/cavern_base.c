/**
 * @file /d/cavern/cavern_base.c
 * @description The base room for the caverns
 *
 * @created 2024-09-18 - Gesslar
 * @last_modified 2024-09-18 - Gesslar
 *
 * @history
 * 2024-09-18 - Gesslar - Created
 */

#include "include/index.h"

inherit STD_ROOM;

void setup() {
  set_light(0);
  set_terrain("road");
}

void virtual_setup(mixed args...) {
  string file = args[0];

  set_zone("winding_caverns");
  CAVERN_DAEMON->setup_room_type(this_object());
  CAVERN_DAEMON->setup_room_subtype(this_object());
  CAVERN_DAEMON->setup_exits(this_object());
  CAVERN_DAEMON->setup_short(this_object());
  CAVERN_DAEMON->setup_long(this_object());
  // debug("Room type for %O is %O", this_object(), query_room_environment());
}
