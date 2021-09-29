/* barckr.c

 Tacitus @ LPUniversity
 10-NOV-05
 LPUniversity Cafe

*/

inherit ROOM;

void create()
{
    set_light(1);
    set_short("A hallway");
    set_long(
      "The room is curved into a hallway that leads to the bathroom.\n"
      "The railing forks, to a buffer area and to the bathroom.\n"
      "A few chairs are positioned into a circle, each chair cushioned\n"
      "with deep red material.\n\n"
      "At the foot of the buffer area, there is a staircase leading\n"
      "up but it is blocked off.\n");
    set_items(([
      "chairs" : "The chairs are cushioned with red material.\n",
      "railing" : "The railing is stained a deep brown.\n",
      "staircase" : "It appears that the staircases lead to an area that is under construction.\n",
    ]) );
    set_exits(([
      "west" : __DIR__ + "bathroom.c",
      "south" : __DIR__ + "cafe.c",
    ]) );
}

