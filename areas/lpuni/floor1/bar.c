/* bar.c

 Tacitus @ LPUniversity
 10-NOV-05
 LPUniversity Cafe

*/ 

inherit ROOM;

void create()
{
    set_light(1);
    set_short("A bar");
    set_long(
      "The railing leads into the room and then turns off to\n"
      "allow full access to the room. A large open area is in\n"
      "the corner for dancing, and a bar spans across the west wall.\n"
      "The bar is stained deep brown to match the railing, it's top\n"
      "a translucent black glass. Tall stools are positioned at\n"
      "spaced intervals along the bar.\n");
    set_items(([
      "railing" : "The railing only enters the room a little bit before\n"
      "breaking off to allow full access to the room. The railing is\n"
      "stained a deep brown.\n",
      "bar" : "The bar is stained a deep brown and the top is made of a\n"
      "translucent, dark black, glass.\n",
      "stools" : "The stools are tall, and are cushioned with black padding.\n",
    ]) );
    set_exits( ([
      "east" : __DIR__ + "cafe.c",
    ]) );
}
