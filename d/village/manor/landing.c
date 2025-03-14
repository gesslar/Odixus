/**
 * @file /d/village/house/landing.c
 * @description The landing in the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

inherit __DIR__ "manor";

void setup() {
  set_short("The Landing");
  set_long("You arrive at a grand landing that serves as a transition between "
  "the two sets of stairs. The space is adorned with a large, gaudy rug that "
  "clashes with the opulent decor. Ornate columns rise to the ceiling, which "
  "is painted with a mural of mythical creatures in vibrant colours. The air "
  "is filled with the scent of incense, creating an atmosphere that is both "
  "mystical and overwhelming.");

  set_exits(([
    "down": "hall3",
    "up"  : "hall4",
  ]));

  set_items(([
    "rug" :
    "The large, gaudy rug clashes with the opulent decor, adding a touch of "
    "chaos.",
    "columns" :
    "The ornate columns rise majestically to the ceiling, enhancing the "
    "grandeur of the space.",
    "mural" :
    "The mural of mythical creatures is painted in vibrant colours, "
    "captivating the eye.",
    "incense" :
    "The scent of incense fills the air, creating an atmosphere that is both "
    "mystical and overwhelming.",
  ]));
}
