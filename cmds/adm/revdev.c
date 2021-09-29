/* revdev.c

 Tacitus @ LPUniversity
 15-JAN-06
 Admin command

*/

//Last edited on October 6th, 2006 by Tacitus

#include <logs.h>

int main(string args)
{
     object user;
     object security_editor;

     if(!adminp(previous_object())) return notify_fail("Error [redev]: Access denied.\n");
     if(!args) return(notify_fail("Syntax: revdev <user>\n"));

     args = lower_case(args);
     user = find_player(args);

     if(!user) return(notify_fail("Error [makedev]: User '" + args + "' is not online.\n"));
     if(!devp(query_privs(user))) return(notify_fail("Error [makedev]: That user is not a developer.\n"));

     write("Revoking developer access for '" + capitalize(user->query_name()) + "'.\n");
     tell_object(user, "\n%^BOLD%^RED%^Developer Access Revoked.%^RESET%^\n");
     user->rem_path("/cmds/wiz/");
     user->rem_path("/cmds/object/");
     user->rem_path("/cmds/file/");
     user->rem_path("/cmds/adm/");
     security_editor = clone_object("/adm/obj/security_editor.c");
     security_editor->disable_membership(query_privs(user), "developer");
     security_editor->write_state(0);
     destruct(security_editor);
     user->save_user();

     write("Success [revdev]: User '" + capitalize(user->query_name()) + "' is no longer a developer.\n");
     log_file(LOG_PROMOTE, capitalize(query_privs(this_player())) + " revoked " 
        + user->query_cap_name() + "'s developer status on " + ctime(time())
        + "\n");
     return 1;
}

string help()
{
     return(" %^BOLD%^SYNTAX:%^ revdev <user>\n\n"
     "This command will revoke a user's developer access to\n"
     "the mud by removing them from the group 'developer' and\n"
     "by removing the directories '/cmds/wiz/', '/cmds/object/'\n"
     "and '/cmds/file/' from the user's command path.\n\n"
     "%^BOLD%^See also:%^RESET%^ makedev\n");
}
