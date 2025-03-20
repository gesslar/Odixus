/**
 * @file /obj/container/backpack.c
 * @description A small wearable container that allows players to store and carry
 * items. The backpack functions both as a piece of clothing that can be worn and
 * as a container that can hold various objects. It comes with a closure mechanism
 * to secure items inside.
 *
 * @created 2024-08-07 - Gesslar
 * @last_modified 2024-08-07 - Gesslar
 *
 * @history
 * 2024-08-07 - Gesslar - Created
 */

inherit STD_CLOTHING;
inherit STD_CONTAINER;

void setup() {
  set_id("backpack");
  set_adj("small");
  set_name("backpack");
  set_short("a small backpack");
  set_long("This is a sturdy, well-made backpack crafted from durable canvas "
    "material. It features adjustable leather straps, a main compartment with "
    "a secure flap closure, and several smaller pockets for organizing items. "
    "Despite its modest size, it appears capable of holding a reasonable "
    "amount of supplies.");
  set_mass(5);
  set_capacity(10);
  set_closeable(1);
  set_closed(1);
}
