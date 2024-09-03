/**
 * @file /cmds/std/travel.c
 * @description Command to run to common destinations from your current
 *              location. Requires GMCP.
 *
 * @created 2024-08-24 - Gesslar
 * @last_modified 2024-08-24 - Gesslar
 *
 * @history
 * 2024-08-24 - Gesslar - Created
 */

#include <gmcp_defines.h>

inherit STD_CMD ;

private nosave mapping destinations ;
private nosave string *dests ;

void setup() {
    destinations = mud_config("TRAVEL_DESTINATIONS") ;
    destinations = map(destinations, (: chop($2, ".c", -1) :)) ;
    destinations = map(destinations, (: prepend($2, "/") :)) ;

    dests = keys(destinations) ;
    dests = sort_array(dests, 1) ;

    usage_text =
"travel list - List all valid travel destinations.\n"
"travel <destination> - Travel to a common destination.";
    help_text =
"This command allows you to travel to common destinations from your current "
"location.\n\n"
"Valid destinations are: " + implode(dests, ", ") + ".\n\n"
"Please note that you will always pass through the village square when "
"traveling to any destination, regardless of the proximity of your current "
"location to the destination.\n\n"
"You must have GMCP enabled to use this command." ;
}

mixed main(object tp, string str) {
    string *stops ;
    string wp_file ;
    mixed *wps ;
    string *wp ;
    int num ;
    string destination_file, destination_name ;


    if(!get_config(__RC_ENABLE_GMCP__))
        return 0 ;

    if(!tp->gmcp_enabled())
        return "You must have GMCP enabled to use this command." ;

    if(!str)
        return "Travel to where?" ;

    if(str == "list")
        return "Valid destinations are: " + implode(dests, ", ") + "." ;

    if(!of(str, destinations)) {
        printf("Not in destinations\n") ;
        printf("Nullp: %O\n", nullp(num = to_int(str))) ;
        if(!nullp(num = to_int(str))) {
            printf("Is int\n") ;
            printf("Num: %O\n", num) ;
            wp_file = user_data_directory(query_privs(tp)) + "waypoints.txt" ;
            if(file_exists(wp_file)) {
                printf("File exists\n") ;
                wps = restore_variable(read_file(wp_file)) ;
                if(num >= 1 && num <= sizeof(wps)) {
                    printf("Num is valid\n") ;
                    wp = wps[num-1] ;
                    destination_name = wp[0] ;
                    destination_file = wp[1] ;
                }
            }
        } else {
            destination_name = str ;
            destination_file = destinations[str] ;
        }
    }

    if(!destination_file)
        return "That is not a valid destination." ;

    if(base_name(environment(tp)) == destination_file)
        return "You are already at that destination." ;

    tell(tp, "Traveling to " + destination_name + "...\n") ;

    stops = ({ destination_file }) ;

    if(str != "square")
        stops = ({ destinations["square"], destination_file }) ;

    GMCP_D->send_gmcp(tp, GMCP_PKG_ROOM_TRAVEL, stops) ;

    return 1 ;
}
