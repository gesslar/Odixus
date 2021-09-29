/* lounge.c

 Tacitus @ LPUniversity
 10-NOV-05
 LPUniversity Cafe

*/

inherit ROOM;

int i = -1;

void create()
{
    set_light(1);
    set_short("A small lounge");
    set_long(
      "The lounge inherits a feeling of comfort and softness.\n"
      "The room is small, but is constructed and decorated in a\n"
      "way to protray space and to offer a quality environment\n"
      "to just sit and relax. Paintings litter the wall with\n"
      "beauty and couches offer ample opportunity to view them.\n");
    set_items(([
      ({"painting", "paintings"}) : "@@query_paintint",
      ({"couche", "couches"}) : "The couches are coloured royal blue and are very fluffy.\n",
    ]) );

    set_exits( ([
      "west" : __DIR__ "cafe.c",
    ]) );
}

string query_paintint()
{
     string *paint = ({
          "You look at a painting of a large, dark, brown horse jumping into the sky.\n",
          "You look at a painting of a rainbow and a gold raining from the sky.\n",
          "You examine a painting of a stormy night.\n",
          "You look at a painting of a weird machine with streaks of blue.\n",
          "You look at a painting of a vortex of doom!\n",
          "You look at a painting of a cat.\n",
          "You look at a painting of death, people dying everywhere.\n",
          "You look at a painting, it is very large. A man is riding a white horse.\n",
          "You look at a painting of the LPUniversity Founder -- Tacitus.\n",
          "You look at a painting of a cute, small, fluffy animal.\n",
          "You look at a painting of dogs playing poker.\n",
          "You look at a painting of the luxurious LP University.\n",
          "You look at a painting of two friends biking down a busy street.\n",
          "You look at a painting of man. The painting is labeled Icoz.\n",
          "You look at a painting of an old man that has a long beard.\n"
     });

     if (i < 0) i = random(sizeof(paint));
     i += 1;
     if(i > sizeof(paint)-1) i = 0;
     return paint[i];
}