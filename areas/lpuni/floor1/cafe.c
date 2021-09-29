/* cafe.c

 Tacitus @ LPUniversity
 10-NOV-05
 LPUniversity Cafe

*/ 

inherit ROOM;

void create()
{
    set_light(1);
    set_short("A cozy cafe");
    set_long(
      "The cafe continues the trend of victorian age and modern luxery with\n"
      "beautiful arrays of stained tables and chairs. Six sets of tables,\n"
      "each with four cushioned chairs populate the room. Each set divided\n"
      "into groups of two, and positioned on slightly elevated platforms in\n"
      "the bottom left corner, bottom right corner, and top right corner. A\n"
      "serving station is constructed in the top left corner. The oak\n"
      "railing protrudes from the exits and directs traffic once again to\n"
      "a quiet loung in the east, a bar in the west, and a backroom in the north.\n"
      "A small bathroom sign hangs from the ceiling by the north exit.\n");

    set_items( ([
    "tables" : "The tables are stained a deep brown. The tops are depressed, and\n"
    "laying in the depressions are hard, black cushions.\n",
    "chairs" : "The chairs are large and luxurious. They are cushioned with soft\n"
    "black coloured, padding.\n",
    "railing" : "The deep brown stained railing directs and divides the room into\n"
    "organization.\n",
    "serving station" : "The station contains all the estenials to cook with. A\n"
    "small, royal blue, rope ties off the center from the public.\n",
    "station" : "The station contains all the estenials to cook with. A\n"
    "small, royal blue, rope ties off the center from the public.\n",
      ]) );

    set_exits(([
    "south" : "/areas/lpuni/entrance.c",
    "north" : __DIR__ + "backr.c",
    "west"  : __DIR__ + "bar.c",
    "east" : __DIR__ + "lounge.c",
      ]) );

}
