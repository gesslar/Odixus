/**
 * @file /d/village/house/bedroom2.c
 * @description A bedroom in the manor.
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
  set_short("A Bedroom") ;
  set_long("This bedroom is a spectacle of excess. The walls are painted a "
  "shocking pink, and the bed is surrounded by sheer, flowing curtains "
  "that seem to float in the air. A large, gaudy chandelier hangs from the "
  "ceiling, casting a dim light over the room. A collection of mismatched "
  "furniture pieces, all in bright colours, creates a chaotic yet strangely "
  "alluring atmosphere.") ;

  set_exits(([
    "east" : "hall5",
  ])) ;

  add_door(new(class Door,
    id: "bedroom2 door",
    direction: "east",
    short: "A grand door",
    long: "A grand door that leads to the hall."
  )) ;

  set_items(([
    "bed" :
    "The bed is large and inviting, surrounded by sheer, flowing curtains.",
    "chandelier" :
    "The gaudy chandelier hangs from the ceiling, casting a dim light over "
    "the room.",
    "furniture" :
    "The mismatched furniture pieces are all in bright colours, creating a "
    "chaotic yet alluring atmosphere.",
  ])) ;
}
