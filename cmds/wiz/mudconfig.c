// /cmds/wiz/mudconfig.c
// Command to list all the mud configuration options available to
// mud_config() sefun.
//
// Created:     2024/02/19: Gesslar
// Last Change: 2024/02/19: Gesslar
//
// 2024/02/19: Gesslar - Created

#include <daemons.h>

inherit STD_CMD ;

mixed main(object tp, string args) {
    string *out ;
    mapping config ;
    string temp ;

    config = CONFIG_D->get_all_config() ;

    temp = pretty_map(config) ;

    return ({ "Current MUD Configuration", "" }) + explode(temp, "\n") ;
}
