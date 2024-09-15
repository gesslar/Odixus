/**
 * @file /d/village/house/library.c
 * @description A library in the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

inherit __DIR__ "manor" ;

void setup() {
  set_short("The Library") ;
  set_long("This library is a chaotic mix of bookshelves overflowing with "
  "books of all sizes and colours. The walls are painted a deep purple, and "
  "the furniture is a mix of styles, from Victorian to modern. A large "
  "globe sits in one corner, while a collection of bizarre artifacts from "
  "around the world decorates the shelves. The air is thick with the smell "
  "of old paper and dust, creating an atmosphere that is both intriguing "
  "and slightly claustrophobic.") ;

  set_exits(([
    "north" : "conservatory",
  ])) ;

  set_items(([
    "bookshelves" :
    "The bookshelves are overflowing with books, some stacked "
    "haphazardly, creating a chaotic yet charming atmosphere.",
    "globe" :
    "The large globe in the corner is a fascinating artifact, "
    "showing the world in vibrant colours.",
    "artifacts" :
    "Bizarre artifacts from around the world decorate the shelves, "
    "each with its own unique story.",
  ])) ;
}
