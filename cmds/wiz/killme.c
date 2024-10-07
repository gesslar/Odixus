/**
 * @file /cmds/wiz/killme.c
 * @description This command commands all NPCs in a room to attack you. It
 *              does not change their current target if they are already
 *              attacking someone else.
 *
 * @created 2024-10-06 - Gesslar
 * @last_modified 2024-10-06 - Gesslar
 *
 * @history
 * 2024-10-06 - Gesslar - Created
 */

inherit STD_CMD ;

void setup() {
  usage_text = "killme" ;
  help_text =
"This command is used to coerce all NPCs in a room to attack you. It does not "
"change their current target if they are already attacking someone else." ;
}

mixed main(object caller, string args) {
  object *npcs ;

  npcs = present_npcs(environment(caller)) ;
  foreach(object npc in npcs)
    if(npc->is_npc() && !npc->query_invis()) {
      if(npc->start_attack(caller))
        caller->targetted_action("$N $vcommand $t to attack $n.", npc) ;
      else
        tell(caller, sprintf("You cannot command %s to attack you.\n", npc->query_name())) ;
    }

  return 1 ;
}
