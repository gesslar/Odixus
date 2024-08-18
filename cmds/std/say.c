/**
 * @file /cmds/std/say.c
 * @description A command to say something to the room.
 *
 * @created 2024-08-18 - Gesslar
 * @last_modified 2024-08-18 - Gesslar
 *
 * @history
 * 2024-08-18 - Gesslar - Created
 */

inherit STD_ACT ;

void setup() {
    usage_text = "say <message>";
    help_text =
"This command enables you to speak to the room.";
}

mixed main(object tp, string str) {
    if(!str)
        return "Say what?";

    tp->simple_action("$N $vsay, \"$o\"", str);

    return 1;
}
