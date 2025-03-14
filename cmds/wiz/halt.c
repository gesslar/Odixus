// /cmds/wiz/halt.c
// Halt combat
//
// Created:     2024/02/20: Gesslar
// Last Change: 2024/02/20: Gesslar
//
// 2024/02/20: Gesslar - Created

#include <daemons.h>

inherit STD_CMD;

mixed main(object tp, string arg) {
    object *who;
    object room = environment(tp);

    if(!arg) {
        who = ({ tp });
    } else if(arg == "all") {
        who = filter(all_inventory(room), (: living :));
    } else if(arg == "global") {
        who = filter(livings(), (: environment :));
    } else {
        who = ({ present(arg, room) });
    }

    who -= ({ 0 });
    if(!sizeof(who))
        return "No such target.";

    who -= ({ 0 });

    if(sizeof(who) == 0)
        return "No combat to halt.";

    who->stop_all_attacks();

    return "You halt combat for "+
        simple_list(sort_array(who->query_name(), 1))+".";
}
