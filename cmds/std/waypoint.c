/**
 * @file /cmds/std/waypoint.c
 * @description Command for managing waypoints.
 *
 * @created 2024-09-02 - Gesslar
 * @last_modified 2024-09-02 - Gesslar
 *
 * @history
 * 2024-09-02 - Gesslar - Created
 */

inherit STD_CMD ;

void setup() {
  usage_text =
  "waypoint list - List all waypoints.\n"
  "waypoint set - Set a waypoint at your current location.\n"
  "waypoint remove <#> - Remove a waypoint.\n" ;

  help_text =
  "This command allows you to manage waypoints, which are saved locations "
  "that you can quickly travel to using the `travel` command.\n\n"
  "To set a waypoint, type `waypoint set`. This will save your current "
  "location as waypoint 1. You can set additional waypoints by using the "
  "`waypoint set` command again, which will save your current location as "
  "waypoint 2, and so on.\n\n"
  "To remove a waypoint, type `waypoint remove <#>`, replacing <#> with the "
  "number of the waypoint you want to remove.\n\n"
  "To use a waypoint, type: `travel <waypoint #>`.\n\n"
  "You must have GMCP enabled to use this command." ;
}

string list_waypoints(object tp) ;
string set_waypoint(object tp) ;
string remove_waypoint(object tp, int num) ;

mixed main(object tp, string str) {
  string name = query_privs(tp) ;
  string file = user_data_directory(name) + "waypoints.txt" ;
  string *lines = ({});
  int num ;

  if(!str)
    str = "list" ;

  if(str == "list")
    return list_waypoints(tp) ;

  if(str == "set")
    return set_waypoint(tp) ;

  if(sscanf(str, "remove %d", num))
    return remove_waypoint(tp, num) ;

  return usage(tp) ;
}

mixed list_waypoints(object tp) {
  string name = query_privs(tp) ;
  string file = user_data_directory(name) + "waypoints.txt" ;
  int max_waypoints = mud_config("WAYPOINTS_MAX") ;
  mixed *wps = ({});
  int num, sz ;
  string result = "" ;
  string data ;
  string short ;

  if(file_exists(file)) {
    data = read_file(file) ;
    wps = restore_variable(data) ;
  }
  else
    wps = ({}) ;

  sz = sizeof(wps) ;
  wps = array_fill(wps, ({}), max_waypoints-sz) ;
  sz = sizeof(wps) ;
  for(num = 0; num < sz; num++) {
    if(sizeof(wps[num])) {
      short = wps[num][0] ;
      file = wps[num][1] ;
      result += sprintf("%2d: %s\n", num + 1, short) ;
    } else {
      result += sprintf("%2d: %s\n", num + 1, "Unknown") ;
    }
  }

  return result ;
}

mixed set_waypoint(object tp) {
  string name = query_privs(tp) ;
  string file = user_data_directory(name) + "waypoints.txt" ;
  mixed *wps ;
  string *wp ;
  int i, num ;
  string result = "" ;
  int max_waypoints = mud_config("WAYPOINTS_MAX") ;
  object room = environment(tp) ;
  string room_file = base_name(room) ;
  string room_short = room->query_short() ;

  if(file_exists(file)) {
    string data = read_file(file) ;
    wps = restore_variable(data) ;
  } else
    wps = ({}) ;

  i = num = sizeof(wps) ;

  while(i--) {
    wp = wps[i] ;
    if(wp[1] == room_file)
      return sprintf("You already have a waypoint set to %s.", wp[0]) ;
  }

  if(num >= max_waypoints)
    return "You have reached the maximum number of waypoints." ;

  wp = ({ room_short, room_file }) ;
  printf("wps: %O\n", wps) ;
  wps += ({ wp }) ;
  printf("wps: %O\n", wps) ;

  write_file(file, save_variable(wps), 1) ;

  return sprintf("Waypoint %d set to %s.", num+1, room_short) ;
}

mixed remove_waypoint(object tp, int num) {
  string name = query_privs(tp) ;
  string file = user_data_directory(name) + "waypoints.txt" ;
  mixed *wps = ({});
  string *wp ;
  int i ;
  string result = "" ;
  string data ;

  if(file_exists(file)) {
    data = read_file(file) ;
    wps = restore_variable(data) ;
  }

  if(num < 1 || num > sizeof(wps))
    return "Invalid waypoint number." ;

  wp = wps[num-1] ;
  wps = remove_array_element(wps, num-1) ;

  write_file(file, save_variable(wps), 1) ;

  return sprintf("Waypoint %d to %s removed.", num, wp[0]) ;
}
