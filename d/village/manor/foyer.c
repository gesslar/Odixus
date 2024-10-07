/**
 * @file /d/village/house/foyer.c
 * @description The foyer of the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

#include <classes.h>

inherit __DIR__ "manor" ;
inherit CLASS_DOOR ;

void setup() {
  set_short("The Foyer") ;
  set_long("You step into a grand foyer that is both overwhelming and "
  "extravagant. The walls are lined with gold-framed mirrors that reflect "
  "the light from a massive crystal chandelier, casting prismatic colours "
  "across the room. Plush, velvet carpets in garish colours cover the floor, "
  "muffling your footsteps. A life-sized statue of a marble cherub, "
  "holding a golden goblet, greets you at the entrance, while the air is "
  "thick with the scent of expensive perfumes. This is a space designed "
  "to impress, but it feels more like a circus than a home.") ;

  set_exits(([
    "north": "porch",
    "south": "hall1",
    "east" : "salon",
  ])) ;

  set_items(([
    "mirrors" :
    "The gold-framed mirrors reflect the light beautifully, creating a "
    "dazzling effect.",
    "chandelier" :
    "The massive crystal chandelier hangs from the ceiling, casting prismatic "
    "colours across the room.",
    "carpets" :
    "The plush, velvet carpets are in garish colours, adding to the "
    "extravagant feel of the foyer.",
    "statue" :
    "The life-sized statue of a marble cherub holds a golden goblet, adding "
    "a whimsical touch to the entrance.",
  ])) ;

  add_door(new(class Door,
    id: "foyer door",
    direction: "north",
    short: "A grand door",
    long: "A grand door that leads to the porch."
  )) ;

  add_door(new(class Door,
    id: "foyer door",
    direction: "east",
    short: "A grand door",
    long: "A grand door that leads to the salon."
  )) ;
}
