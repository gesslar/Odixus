/**
 * @file /d/village/house/kitchen.c
 * @description A kitchen in the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

inherit __DIR__ "manor" ;

void setup() {
  set_short("The Kitchen") ;
  set_long("The kitchen is a flamboyant display of culinary excess. "
  "Brightly colored appliances stand out against the gaudy wallpaper, "
  "which features oversized fruit patterns. A large island in the center "
  "is cluttered with an array of mismatched utensils and gadgets, while a "
  "collection of cookbooks, each with a different cover, spills off the "
  "shelves. The air is filled with the scent of spices, making it feel "
  "both inviting and overwhelming.") ;

  set_exits(([
    "east" : "hall2",
  ])) ;

  set_items(([
    "appliances" :
    "The brightly colored appliances are a mix of modern and vintage styles, "
    "adding a quirky charm to the kitchen.",
    "island" :
    "The large island is cluttered with various utensils and gadgets, making "
    "it a hub of culinary activity.",
    "cookbooks" :
    "The cookbooks, each with a different cover, are stacked haphazardly, "
    "offering a variety of recipes and culinary inspiration.",
  ])) ;
}
