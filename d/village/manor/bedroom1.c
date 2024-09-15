/**
 * @file /d/village/house/bedroom1.c
 * @description A bedroom in the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

#include <classes.h>

inherit __DIR__  "manor" ;
inherit CLASS_DOOR ;

void setup() {
  set_short("A Bedroom") ;
  set_long("This bedroom is a cacophony of colors and patterns. The bed is "
  "draped in a bright orange satin coverlet, surrounded by gaudy floral "
  "wallpaper that seems to vibrate. A large, ornate mirror with a gold "
  "frame hangs above a dresser cluttered with trinkets, including a "
  "collection of porcelain cats. The air is heavy with the scent of "
  "lavender, but it feels more overwhelming than soothing.") ;

  set_exits(([
    "south": "hall5",
  ])) ;

  add_door(new(class Door,
    id: "bedroom1 door",
    direction: "south",
    short: "A grand door",
    long: "A grand door that leads to the hall."
  )) ;

  set_items(([
    "bed" :
    "The bed is large and inviting, covered in a bright orange satin "
    "coverlet that catches the eye.",
    "mirror" :
    "The ornate mirror has a gold frame, reflecting the vibrant "
    "colors of the room.",
    "dresser" :
    "The dresser is cluttered with various trinkets, including a collection "
    "of porcelain cats.",
  ])) ;
}
