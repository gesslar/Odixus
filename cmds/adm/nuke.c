/* nuke.c

Tacitus @ LPUniversity
05-MAY-06
Standard nuke command V2

*/

/* Last edited on October 6th, 2006 by Tacitus */

#include <logs.h>

void confirm_nuke(string str, string user);

mixed main(object caller, object room, string user) {
     if(!adminp(previous_object()))
          return "Error [nuke]: Access denied.\n" ;

     if(!user) {
          write("Usage: nuke <player>\n");
          return 1;
     }

     user = lower_case(user);

     if(!user_exists(user)) {
          write("Error [nuke]: User '" + user + "' does not exist.\n");
          return 1;
     }

     write("Are you sure you want to delete " + user + "? [y/n] ");
     input_to("confirm_nuke", 0, caller, user);

     return 1;
}

void confirm_nuke(string str, object caller, string user)
{
     object security_editor;
     object body;
     string *dir;

     if(str == "y" || str == "yes") {
          write("\nWarning [nuke]: Now stripping user of system group memberships.\n");

          security_editor = clone_object("/adm/obj/security_editor.c");

          foreach(mixed group in security_editor->listGroups()) {
               if(isMember(user, group)) write("\t* " + group + " membership revoked.\n");
               security_editor->disable_membership(user, group);
          }

          security_editor->write_state(0);
          destruct(security_editor);

          if(find_player(user)) {
              write("Warning [nuke]: Now disconnecting user '" + user + "'.\n");
               body = find_player(user);
               tell_object(body, "You watch as your body dematerializes.\n");
               if(environment(body)) tell_room(environment(body), "You watch as " + capitalize(user) + " dematerializes before your eyes.\n", body);
               body->remove();
          }

          write("Warning [nuke]: Now deleting pfile for user '" + user + "'.\n");

          dir = get_dir(user_data_directory(user));
          foreach(string file in dir) {
              write("\t* " + file + " deleted.\n");
              if(!rm(user_data_directory(user) + file)) return(notify_fail("\nError [nuke]: Error while deleting " + file + " in user's data directory.\n"));
          }
          rmdir(user_data_directory(user));

          write("\nSuccessful [nuke]: User '" + capitalize(user) + "' has been removed.\n");
          log_file(LOG_NUKE, capitalize(query_privs(caller)) + " nukes "
                + capitalize(user) + " on " + ctime(time()) + "\n");
          return;
     }

     else {
          write("Warning [nuke]: Aborting nuke.\n");
          return;
     }
}

string help(object caller) {
     return (" SYNTAX: nuke <username>\n\n" +
     "This command will delete the target user's pfile, thus \n"
     "removing their account from " + mud_name() + ". Furthermore\n"
     "it will also remove their membership from all groups.\n"
     "This is NON REVERSABLE, so use with discretion.\n\n"
     " See also: lockdown\n");
}
