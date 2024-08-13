/**
 * @file /std/modules/mobile/race/ghost.c
 * @description Ghost racial module
 *
 * @created 2024-07-25 - Gesslar
 * @last_modified 2024-07-25 - Gesslar
 *
 * @history
 * 2024-07-25 - Gesslar - Created
 */

inherit __DIR__ "race" ;

int set_up_body_parts(object ob, mixed args...) {
    wipe_body_parts() ;
    return 1 ;
}
