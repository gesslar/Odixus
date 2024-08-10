/**
 * @file /cmds/wiz/exits.c
 * @description Shows all the exits in the current room and where they lead.
 *
 * @created 2024/02/04 - Gesslar
 * @last_modified 2024/02/04 - Gesslar
 *
 * @history
 * 2024/02/04 - Gesslar - Created
 */

inherit STD_CMD ;

void setup() {
    usage_text = "exits";
    help_text =
"Shows all the exits in the current room and where they lead.";
}

mixed main(object tp, string args) {
    string *exits;
    string str;
    int i;
    mapping x ;
    object room = environment(tp);

    exits = keys(room->query_exits());
    if(!sizeof(exits))
        return "There are no exits from this room.";

    x = allocate_mapping(exits, (:
        $(room)->query_exit($1)
    :));

    str = "Exits:\n";
    for(i = 0; i < sizeof(exits); i++) {
        str += exits[i] + " = " + x[exits[i]] + "\n";
    }

    return str;
}
