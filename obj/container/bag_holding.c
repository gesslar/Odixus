/**
 * @file /obj/container/bag_holding.c
 * @description A bag holding object
 *
 * @created 2024-08-07 - Gesslar
 * @last_modified 2024-08-07 - Gesslar
 *
 * @history
 * 2024-08-07 - Gesslar - Created
 */

inherit STD_ITEM;
inherit STD_CONTAINER;

void setup() {
    set_id("bag");
    set_adj("small");
    set_name("bag");
    set_short("a small bag");
    set_long("A bag for carrying items.");
    set_mass(5);
    set_capacity(200);
    set_ignore_mass(1);

    set_closeable(1);
    set_closed(1);
}
