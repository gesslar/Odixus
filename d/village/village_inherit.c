/**
 * @file /d/village/village_inherit.c
 * @description General inherit for the village area
 *
 * @created 2024-02-04 - Gesslar
 * @last_modified 2024-02-04 - Gesslar
 *
 * @history
 * 2024-02-04 - Gesslar - Created
 */

inherit STD_ROOM ;

void pre_setup_1() {
    set_zone(__DIR__ "olum") ;
    set_terrain("road") ;
}
