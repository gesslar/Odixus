/**
 * @file /cmds/std/idea.c
 * @description Command for reporting ideas.
 *
 * @created 2024-07-13 - Gesslar
 * @last_modified 2024-07-13 - Gesslar
 *
 * @history
 * 2024-07-13 - Gesslar - Created
 */


#include <daemons.h>

inherit STD_REPORTER;

void finish_report(string text, object tp);

void setup() {
    set_report_type(query_file_name());
    set_git_hub_label("enhancement");
}
