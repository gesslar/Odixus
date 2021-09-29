/* clean.c

Icoz@LPUniversity (ico2ico2@gmail.com)
28-OCT-05
Object minipulation command

QC Review: Tacitus on October 28th, 2005

Last edited on October 29th, 2005 by Tacitus

*/

int main(string arg)
{
     int i;
     object target;
     object *inventory;

     if(!arg) target = this_player();
     else
     {
          if(arg[0]!='/') arg = resolve_path(this_player()->query("cwd"), arg);
          if(arg[<2..<1] != ".c") arg += ".c";
          if(!target) target = find_object(arg);
          if(!target) target = present(arg, this_player());
          if(!target) target = present(arg, environment(this_player()));
     }

     if(!target) return notify_fail("Error [clean]: Error locating target.\n");
     write("Success [clean]: Destryoing all objects in '" + target->query_short() + "'.\n");
     inventory = all_inventory(target);

     for(i=0; i<sizeof(inventory); i++)
     {
          if(inventory[i]->query("no_clean") || inventory[i]->clean_up()) continue;
          if(living(inventory[i])) continue;

          write("\tObject '" + inventory[i]->query_short() + "' destroyed.\n");
          inventory[i]->remove();

     }

     return 1;
}

string help()
{
     return(HIW + " SYNTAX: " + NOR + "clean <object>\n\n"
     "This command will destory all objects within another object (aka\n"
     "the objects inventory). Your argument may be an object that is\n"
     "in your inventory, your environment, or a filename. If you do\n"
     "not speicfy an object, it will remove your inventory.\n");
}
