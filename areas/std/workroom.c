/* workroom.c

Tacitus @ LPUniversity
23-OCT-05
Generic Workroom

This is file demonstates the mudlib's room inheritable
basic capabilities and mudlib coding standards. If you
do not know how to edit this file to produce your own
personalized workroom, then please refer to the mudlib,
lpc, and education documentation or contact a more senior
developer.

Regarding the room inheritable, it is compatable with
the two most common used methods used in today's mudlib.
 a) set_<prop>(<value);
 b) set(<prop>, <value>);

The recommended and supported method is set_<prop>(<value>);
if there is one available. The second method is ment for
non-standard values and backwards area support.

*/

/* Last edited on July 11th, 2006 by Tacitus */

inherit ROOM;

void create()
{
     set_light(1);
     set_short("A workroom");
     set_long(
          "This is a bare and generic workroom for developers who don't\n"
          "have one of their own. To see the source code for this\n"
          "workroom, you can type 'more here'. There is a sign on the\n"
          "wall.\n"
     );
     set_exits( ([
          "cafe" : "/areas/lpuni/entrance.c",
          "void" : "/areas/std/void.c"
     ]) );
     set_items( ([
          "sign" : "The sign reads:\n"
               "'You are never given a wish without also being given the\n"
               "power to make it come true. You may, however, have to work for\n"
               "it.'\n",
     ]) );
}

