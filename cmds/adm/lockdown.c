/* lockdown.c

 Parthenon @ LPUniversity
 26-JUN-2006
 LPUni mud availablitly command

*/

//Last edited on July 17th, 2006 by Parthenon

#define LOCKDOWN_TOOL "/obj/mudlib/lockdown_admin/lockdown_client.c"

int main(object caller, object room, string arg)
{
    object lockdown_client;

    if(!adminp(previous_object())) return(notify_fail("Error [access]: Access denied.\n"));

    lockdown_client = clone_object(LOCKDOWN_TOOL);
    lockdown_client->move(caller);
    lockdown_client->main();
    return 1;
}

string help(object caller)
{
    return(
      " SYNTAX: access\n\n"+
      "This command allows you as the admin to control whether or not\n"+
      "the mud is locked down. You can allow only admins, or only devs\n"+
      "and above. In addition, you may also add names or IP addresses\n"+
      "to ban lists. Also, if you wish to open the mud only to certain\n"+
      "players such as for play-testing something, you may also do that.\n");
}
