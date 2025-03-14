/**
 * @file /d/village/house/washroom.c
 * @description A washroom in the manor.
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
  set_short("The Washroom");
  set_long("This washroom is a curious blend of luxury and absurdity. The "
  "walls are covered in shiny, reflective tiles that seem to amplify the "
  "light, creating a dazzling effect. A large, ornate mirror hangs above a "
  "lavish sink, surrounded by an array of colourful soaps and lotions that "
  "look more like decorations than practical items. The air is thick with "
  "the scent of floral fragrances, making it feel more like a spa than "
  "a washroom.");

  set_exits(([
    "west" : "hall4",
  ]));

  add_door(new(class Door,
    id: "washroom door",
    direction: "west",
    short: "A grand door",
    long: "A grand door that leads to the hall."
  ));

  set_items(([
    "countertops" :
    "The marble countertops are smooth and cool to the touch, adding a touch "
    "of luxury.",
    "mirror" :
    "The large mirror reflects the room, enhancing the sense of space and "
    "opulence.",
    "candles" :
    "The scented candles fill the air with a fragrant aroma, creating a "
    "relaxing atmosphere.",
    "towels" :
    "The plush towels are neatly arranged, inviting you to indulge in a "
    "moment of luxury.",
    "bathtub" :
    "The large bathtub is inviting, surrounded by potted plants that add a "
    "touch of greenery.",
  ]));
}
