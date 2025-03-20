/**
 * @file /obj/container/iron_chest.c
 * @description A sturdy iron chest that can be locked and unlocked. This
 * container provides secure storage for valuable items and can be placed in
 * rooms or carried, though its significant weight makes transport challenging.
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
  set_id(({"chest", "iron chest"}));
  set_adj("iron");
  set_name("chest");
  set_short("an iron chest");
  set_long("This heavy iron chest is built to last, with thick metal walls "
    "and reinforced corners. The lid is secured by a robust iron hasp and "
    "a heavy-duty lock mechanism. The chest's surface bears a weathered patina, "
    "though the hinges appear well-oiled and functional. Despite its "
    "utilitarian design, there are subtle decorative flourishes worked into "
    "the metalwork.");
  set_mass(50);
  set_capacity(100);
  set_closeable(1);
  set_closed(1);
  set_lockable(1);
  set_locked(1);
  set_key_id("manor key");
}
