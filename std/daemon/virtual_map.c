/**
 * @file /std/daemon/virtual_map.c
 * @description Daemon responsible for managing virtual maps for a zone.
 *              All coordinates are in the format of z,y,x.
 *
 * @created 2024-08-23 - Gesslar
 * @last_modified 2024-08-23 - Gesslar
 *
 * @history
 * 2024-08-23 - Gesslar - Created
 */

inherit STD_DAEMON;

// Forward declarations
public void set_map_file(string file) ;
public void load_map() ;
public int is_valid_room(int z, int y, int x) ;
public int get_map_width() ;
public int get_map_height() ;
public string *get_directions() ;
public mapping get_room_info(int z, int y, int x) ;

private nosave string map_type ;
private nosave string map_file ;
private nosave mixed *map_data ;
private nosave string *map_lines ;
private nosave int map_width, map_height, map_depth ;
private nosave function noise_map ;
private nosave mixed noise_range = ({ MAX_FLOAT, -MAX_FLOAT }) ;

private nosave string *directions = ({"north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"}) ;
private nosave int *dx = ({0, 1, 1, 1, 0, -1, -1, -1}) ;
private nosave int *dy = ({-1, -1, 0, 1, 1, 1, 0, -1}) ;
private nosave int *exit_symbols = ({'|', '/', '-', '\\', '|', '/', '-', '\\'});

public void apply_map_file(string file) {
  if(!file)
    error("No map file set. Use set_map_file() first.");

  map_type = "file";
  map_file = file;
  map_lines = explode(read_file(map_file), "\n");
  map_height = sizeof(map_lines);
  map_width = max(map(map_lines, (: strlen :)));
  map_depth = 1 ;
}

// An indeterminate map is one that is generated on demand and has no fixed
// size or shape and therefore cannot be stored in memory. All functions that
// query the map for data will return null. You should accommodate for this
// in your own object.
protected varargs void apply_map_generator(function f, int indeterminate) {
  mixed result ;

  if(!valid_function(f))
    error("Invalid map generator function.");

  noise_map = f;
  result = (*f)() ;

  if(!indeterminate)
    if(!result || !pointerp(result))
      error("Map generator function did not return a valid map.");

  if(indeterminate == true) {
    map_type = "squirrel5";
    map_depth = -1;
    map_data = null ;
  } else {
    map_type = "generator";

    map_data = result ;
    map_depth = sizeof(map_data) ;
    map_height = sizeof(map_data[0]) ;
    map_width = sizeof(map_data[0][0]) ;
  }
}

string get_map_type() {
  return map_type;
}

protected void update_noise_range(mixed value) {
  if(nullp(value))
    return ;

  noise_range[0] = min(({value, noise_range[0]})) ;
  noise_range[1] = max(({value, noise_range[1]})) ;
}

public mixed *get_noise_range() {
  return noise_range ;
}

mixed *query_map_data() {
  return map_data ;
}

protected mapping get_exits(int z, int y, int x) {
  mapping exits = ([]);
  int i, nx, ny ;

  if(map_type == "file") {
    for(i = 0; i < sizeof(directions); i++) {
      nx = x * 2 + dx[i];
      ny = y * 2 + dy[i];

      if(ny >= 0 && ny < map_height && nx >= 0 && nx < map_width) {
        if(sizeof(map_lines[ny]) > nx && map_lines[ny][nx] == exit_symbols[i]) {
          exits[directions[i]] = sprintf("%d,%d,%d", x + dx[i], y + dy[i], z);
        }
      }
    }
  } else if(map_type == "generator") {
    for(i = 0; i < sizeof(directions); i++) {
      if(y >= 0 && y < map_height && x >= 0 && x < map_width) {
        if(map_data[y][x] > 0.5) {
          exits[directions[i]] = sprintf("%d,%d,%d", x + dx[i], y + dy[i], z);
        }
      }
    }
  } else if(map_type == "indeterminate") {
    return null ;
  }

  return exits;
}

/* protected */ mixed get_room_type(int z, int y, int x) {
  if(map_type == "file") {
    if(y * 2 < map_height && x * 2 < map_width)
      return map_lines[y * 2][x * 2..x * 2];
  } else if(map_type == "generator") {
    if(y >= 0 && y < map_height && x >= 0 && x < map_width && z >= 0 && z < map_depth)
      return map_data[z][y][x];
  } else if(map_type == "indeterminate") {
    return null ;
  }

  return null ;
}

public int is_valid_room(int z, int y, int x) {
  mixed room_type ;

  if(map_type == "indeterminate")
    return 1 ;

  room_type = get_room_type(z, y, x);

  return (room_type && room_type != "" && room_type != " ");
}

public int get_map_width() {
  if(map_type == "file")
    return map_width / 2 ;
  else if(map_type == "indeterminate")
    return 1 ;
  else
    return map_width;
}

public int get_map_height() {
  if(map_type == "file")
    return map_height / 2 ;
  else if(map_type == "indeterminate")
    return 1 ;
  else
    return map_height;
}

public int get_map_depth() {
  if(map_type == "indeterminate")
    return 1 ;
  else
    return map_depth;
}

public string *get_directions() {
  return directions;
}

public mapping get_room_info(int z, int y, int x) {
  return ([
    "type": get_room_type(z, y, x),
    "exits": get_exits(z, y, x),
  ]);
}
