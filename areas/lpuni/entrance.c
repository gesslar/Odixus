/* entrance.c

 Tacitus @ LPUniversity
 10-NOV-05
 LPUniversity Cafe

*/

inherit STD_ROOM;

void create() {
    object mob = new(__DIR__ + "mobiles/greeter.c");
    set_light(1);
    set_short("The entrance to LPUniversity cafe");
    set_long(
      "The room is an elaborate combination of victorian age and modern luxury.\n"
      "The floor is glossed marble, the walls papered a soft pink. Massive\n"
      "granite pillars tower to the ceiling of perplexing glasses. Light shines\n"
      "diligently, shaded and coloured from the colourful perplexity above.\n"
      "Traffic is directed by sturdy, stained brown, railing that leads to the\n"
      "luxurious array of tables and chairs to the north. A podium, similarly\n"
      "stained, is positioned by the entrance to the eatery.\n");
    set_items( ([
    "floor" : "The floor is heavily polished, glossed marble.\n",
    "walls" : "The walls have a soft pink wall paper.\n",
    "railing" : "The railing is stained a deep brown and are made of sturdy oak.\n",
    "podium" : "The podium is stained a deep brown to match the railing, it retains high flavour with\n"
    "it's elaborate designs on the sides.\n",
      ]) );
    set_exits(([
    "north" : __DIR__ + "floor1/cafe.c",
      ]) );
    mob->move(this_object());
}
