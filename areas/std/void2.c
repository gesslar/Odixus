//void2.c

//Tacitus @ LPUniversity
//29-JUNE-05
//The Void # 2

//Last updated June 9th, 2006 by Tacitus

inherit ROOM;

void create()
{
     set_light(1);
     set_short("The void2");
     set_long("This is the second room to ever exists on LPUniversity!\n");
     set_exits((["void" : "/areas/std/void.c"]));
     set_items((["test" : "Test test test!\n"]));
}