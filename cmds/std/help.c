/* help.c

* Tacitus @ LPUniversity
* 08-OCT-05
* Standard Command

* -> First attempt at a help system <-

*/

//Last Edited on October 6th, 2006 by Tacitus

inherit CMD ;

#define HELP_PATH ({"/doc/general/", "/doc/game/"})
#define DEV_PATH ({"/doc/wiz/", "/doc/driver/efun/all/", "/doc/driver/applies/", "/doc/driver/lpc/constructs/", "/doc/driver/lpc/types/",})
#define BORDER HIW + "[=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=]" + NOR + "\n"

#include <logs.h>

int main(object caller, object room, string str) {
    string file, *path, err, output = "";
    object cmd, pager;
    int i;

    pager = clone_object(OBJ_PAGER);
    if(!str) str = "help";
    path = this_player()->query_path();

     for(i = 0; i < sizeof(path); i++) {
        if(file_exists(path[i] + str + ".c")) {
            err = catch(cmd = load_object(path[i] + str));
            if(!err) file = cmd->query_help(caller);

            if(err) return notify_fail("Error [help]: This is a problem with "+ str + "\nPlease inform an admin.\n\n");

            if(!file) return notify_fail("Error [help]: The command " + str +
                " exists but there is no help file for it.\n"
                " Please inform an admin.\n\n");

            output += BORDER;
            output += ("\t\t %^BOLD%^YELLOW%^ Help file for command '%^RESET%^MAGENTA%^"+  capitalize(str) + "%^RESET%^BOLD%^YELLOW%^'\n");
            output += BORDER + "\n";
            output += (file + "\n");

            pager->page(output);
            return 1;
        }
    }

    path = HELP_PATH;
    if(devp(this_player())) path += DEV_PATH;
    if(adminp(this_player())) path += ({"/doc/admin/"});

    for(i = 0; i < sizeof(path); i++) {
        if(file_exists(path[i] + str)) {
            file = read_file(path[i] + str);
            output += BORDER;
            output += ("\t\t %^BOLD%^YELLOW%^ Help file for topic '%^RESET%^MAGENTA%^"+  capitalize(str) + "%^RESET%^BOLD%^YELLOW%^'\n");
            output += BORDER + "\n";
            output += (file + "\n");

            pager->page(output);
            return 1;
        }
    }
    log_file(LOG_HELP, "Not found: " + str + "\n");
    return notify_fail("Error [help]: Unable to find helpfile for: " + str + "\n");
}

string help(object caller) {
    return
"Syntax: help <topic>\n\n"
"Whenever you need help or information regarding something in the mud, this "
"is the place to come. This command gives you instant access to a wealth of "
"information that will be vital to your stay here on " + mud_name() + ". "
"Help that you want not written yet? Let us know and we'll get right on it!" ;
}
