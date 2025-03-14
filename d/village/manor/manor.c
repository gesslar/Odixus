/**
 * @file /d/village/house/manor.c
 * @description Inheritable for the opulent manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

inherit STD_ROOM;

void base_setup() {
  set_zone("opulent_manor");
  set_terrain("indoor");
}
