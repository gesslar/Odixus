/**
 * @file /adm/daemons/message.c
 * @description Message routines and functions
 *
 * @created 2024-07-28 - Gesslar
 * @last_modified 2024-07-28 - Gesslar
 *
 * @history
 * 2024-07-28 - Gesslar - Created
 */

inherit STD_DAEMON;

void setup() {
    set_no_clean(1);
}

string get_message(string type, mixed arg...) {
    string module = "/adm/daemons/modules/message/" + type + ".c";

    if(!file_exists(module))
        return 0;

    return call_other(module, "get_message", arg...);
}
