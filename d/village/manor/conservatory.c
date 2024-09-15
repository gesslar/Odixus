/**
 * @file /d/village/house/conservatory.c
 * @description A conservatory in the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */


inherit __DIR__ "manor" ;

void setup() {
  set_short("A Conservatory") ;
  set_long("The conservatory is filled with exotic plants, each more bizarre "
  "than the last. Brightly coloured flowers bloom in abundance, and the air is "
  "thick with humidity. A fountain in the center, adorned with goldfish, "
  "bubbles softly, while a collection of mismatched garden furniture "
  "surrounds it. The glass ceiling allows sunlight to pour in, creating a "
  "greenhouse effect that feels both tropical and overwhelming.") ;

  set_exits(([
    "west" : "hall2",
    "south" : "library",
  ]));

  set_items(([
    "plants" :
    "The exotic plants are a riot of colours and shapes, each more bizarre "
    "than the last.",
    "flowers" :
    "Brightly coloured flowers bloom in abundance, adding to the vibrant "
    "atmosphere.",
    "fountain" :
    "The fountain bubbles softly, adorned with goldfish that swim lazily in "
    "the water.",
    "furniture" :
    "The mismatched garden furniture creates a casual, inviting space to "
    "relax.",
  ])) ;
}
