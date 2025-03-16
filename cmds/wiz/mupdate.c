/**
 * @file /cmds/wiz/mupdate.c
 * @description This command will cause the caller to be moved around
 *              all coordinates in order to generate a new Mudlet map.
 *
 * @created 2024-08-23 - Gesslar
 * @last_modified 2024-08-23 - Gesslar
 *
 * @history
 * 2024-08-23 - Gesslar - Created
 */

#include <classes.h>
#include <gmcp_defines.h>

inherit STD_CMD;

inherit CLASS_ROOMINFO;

void move_to_room(object tp, string *rooms, mapping data);

private nosave int done, sz;
private nosave float delay = 0.05;

mixed main(object tp, string arg) {
  mapping rooms = COORD_D->get_coordinate_data();
  string *room_names = keys(rooms);

  sz = sizeof(room_names);
  done = 0;

  // room_names = sort_array(room_names, 1);
  room_names = sort_array(room_names, function(string a, string b, mapping data) {
    if(data[a].coords[2] != data[b].coords[2])
      return data[b].coords[2] - data[a].coords[2]; // Sort z from highest to lowest

    if(data[a].coords[1] != data[b].coords[1])
      return data[a].coords[1] - data[b].coords[1]; // Sort y from lowest to highest

    return data[a].coords[0] - data[b].coords[0]; // Sort x from lowest to highest
  }, rooms);

  move_to_room(tp, room_names, rooms);

  return 1;
}

void move_to_room(object tp, string *rooms, mapping data, int it) {
  string room_file;
  object room;
  int *coords;

  room_file = pop(ref rooms);
  // debug_message(room_file);
  // room_file  = element_of(rooms);

  coords = data[room_file].coords;

  done++;

  // tell(tp, sprintf("Sending GMCP Room Info for %s (x: %d, y: %d, z: %d)\n", room_file, coords...));

  if(room = find_object(room_file))
    room->remove();

  GMCP_D->send_gmcp(tp, GMCP_PKG_ROOM_INFO, load_object(room_file));

  if(sizeof(rooms) > 0) {
    if(++it == 250) {
      it = 0;
      call_out_walltime((:move_to_room, tp, rooms, data, it:), delay);
      tell(tp, sprintf("Done %s/%s (%.2f%%)\n", add_commas(done), add_commas(sz - 1), percent(done*1.0, sz*1.0)));
    } else {
      call_out_walltime((:move_to_room, tp, rooms, data, it:), delay);
    }
  } else {
    tell(tp, sprintf("Done %s/%s (%.2f%%)\n", add_commas(done), add_commas(sz - 1), percent(done*1.0, sz*1.0)));
  }
}
