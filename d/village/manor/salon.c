/**
 * @file /d/village/house/salon.c
 * @description A salon in the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

#include <classes.h>

inherit __DIR__ "manor";
inherit CLASS_DOOR;

void setup() {
  set_short("The Salon");
  set_long("The salon is a riot of colour and texture, with walls draped in "
  "mismatched fabrics that clash violently. A grand piano, painted in a "
  "gaudy shade of pink, sits in one corner, while oversized, plush couches "
  "in animal prints dominate the space. The ceiling is adorned with faux-"
  "gold leaf, and a large painting of a sunset hangs crookedly on the wall, "
  "its colours almost blinding. This room is a testament to the residents'"
  "questionable taste.");

  set_exits(([
    "west" : "foyer",
  ]));

  set_items(([
    "furniture" :
    "The plush seating is covered in rich fabrics, inviting guests to relax "
    "and enjoy.",
    "tapestries" :
    "The intricate tapestries on the walls depict scenes of grandeur and "
    "elegance.",
    "piano" :
    "The grand piano in the corner is a beautiful instrument, adding a touch "
    "of class to the salon.",
    "art pieces" :
    "The collection of fine art pieces enhances the luxurious atmosphere of "
    "the room.",
  ]));

  add_door(new(class Door,
    id: "salon door",
    direction: "west",
    short: "A grand door",
    long: "A grand door that leads to the foyer."
  ));
}
