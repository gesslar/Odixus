/**
 * @file /d/village/house/porch.c
 * @description A porch in the manor.
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
  set_short("The Porch");
  set_long("The porch is adorned with gaudy decorations, including bright "
  "plastic flowers and oversized lawn ornaments. A collection of mismatched "
  "chairs and tables creates a chaotic seating arrangement, while a large, "
  "colorful welcome mat greets visitors. The air is filled with the sounds "
  "of chirping birds, but the decor feels more like a carnival than a home.") ;

  set_exits(([
    "north" : "../village_path4",
    "south" : "foyer",
  ])) ;

  set_items(([
    "decorations" :
    "The gaudy decorations include bright plastic flowers and oversized lawn "
    "ornaments, adding a whimsical touch.",
    "chairs" :
    "The mismatched chairs create a chaotic seating arrangement, inviting "
    "visitors to sit and relax.",
    "welcome mat" :
    "The large, colorful welcome mat greets visitors with a cheerful design.",
  ])) ;

  add_door(new(class Door,
    id: "porch door",
    direction: "south",
    short: "A grand door",
    long: "A grand door that leads into the manor."
  )) ;
}
