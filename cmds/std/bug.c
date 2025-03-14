/**
 * @file /cmds/std/bug.c
 * @description Bug command for reporting bugs.
 *
 * @created 2024-07-07 - Gesslar
 * @last_modified 2024-07-07 - Gesslar
 *
 * @history
 * 2024-07-07 - Gesslar - Created
 */

#include <daemons.h>

inherit STD_REPORTER;

void finish_report(string text, object tp);

void setup() {
    set_report_type(query_file_name());
    set_git_hub_label(query_file_name());
}
