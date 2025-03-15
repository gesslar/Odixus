/**
 * @file /d/wastes/wastes_daemon.c
 * @description Virtual map daemon for the wastes
 *
 * @created 2024-08-30 - Gesslar
 * @last_modified 2024-08-30 - Gesslar
 *
 * @history
 * 2024-08-30 - Gesslar - Created
 */

inherit STD_VIRTUAL_MAP;

inherit M_NOISE;

private void setup_wastes_shorts();
private void setup_wastes_longs();
private void generate_map();
private mixed *generate_terrain();
private mixed *generate_river(mixed *map);
varargs string determine_room_type(int z, int y, int x);
private void setup_dimensions();
private void print_row(int y);

private nosave mapping short_descriptions;
private nosave mapping long_descriptions;

private nosave int *dimensions;
// private nosave mixed *dimension_config = ({ ({ 25, 1 }), ({ 25, 1 }),  });
// private nosave mixed *dimension_config = ({ ({ 30, 1 }), ({ 50, 1 }),  });
// private nosave mixed *dimension_config = ({ ({ 30, 1 }), ({ 30, 1 }) });
private nosave mixed *dimension_config = ({ ({ 10, 5 }), ({ 10, 5 }) });
private nosave int WIDTH = 0, HEIGHT = 1;

// The seed is used to ensure that every time the daemon runs, it uses the same
// seed, resulting in the same map.

// Uncomment this to use the boot number as the seed.
private nosave mixed seed = BOOT_D->query_boot_number();

// Use this seed for testing purposes, or whatever you want.
// private nosave mixed seed = 42;

void setup() {
  set_no_clean(1);
  setup_dimensions();

  // Initialize the noise module with the seed, updating our seed with the
  // result.
  seed = init_noise(dimensions[0], dimensions[1], seed);

  // Apply the map generator. This will call the generate_map function and
  // store the result in the inherited map_data.
  apply_map_generator((: generate_map :));

  // Setup the short and long descriptions.
  setup_wastes_shorts();
  setup_wastes_longs();
}

// Set the dimensions of the map, using the seed to generate the dimensions.
private void setup_dimensions() {
  mixed *result;

  dimensions = allocate(2);

  result = prandom(seed, dimension_config[WIDTH][1]);
  seed = result[0];
  dimensions[WIDTH] = result[1] + dimension_config[WIDTH][0];

  // Height - 1 - 5
  result = prandom(seed, dimension_config[HEIGHT][1]);
  seed = result[0];
  dimensions[HEIGHT] = result[1] + dimension_config[HEIGHT][0];
}

// Generate and return the map to the parent virtual_map.
protected mixed *generate_map() {
  mixed *map = generate_terrain();
  map = generate_river(map);

  return map;
}

// Generates the terrain of the map and returns it to the generate_map function.
// This function uses simplex noise to generate the terrain.
private mixed *generate_terrain() {
  mixed map;
  int center_x = dimensions[WIDTH] / 2;
  int center_y = dimensions[HEIGHT] / 2;

  // Create the map. Generate a 3D array, however, we only have one layer.
  // This is stored in the inherited map_data variable, which expects a 3D
  // array.
  map = allocate(1);
  for(int z = 0; z < 1; z++) {
    map[z] = allocate(dimensions[HEIGHT]);
    for(int y = 0; y < dimensions[HEIGHT]; y++) {
      map[z][y] = allocate(dimensions[WIDTH]);
      for(int x = 0; x < dimensions[WIDTH]; x++) {
        mixed *result;
        float noise_value;

        // Dampening function to make the terrain more natural.
        float distance = sqrt(pow(x - center_x, 2) + pow(y - center_y, 2));
        // Adjust the 0.05 value to control strength
        float dampening_factor = 1.0 / (1.0 + distance * 0.05);

        // Generate the noise value.
        noise_value = simplex2(
          to_float(x) / 25.0,
          to_float(y) / 15.0
        );

        // Apply the dampening factor to the noise value.
        noise_value *= dampening_factor;

        // Add 1.0 to the noise value to ensure it is always positive.
        // The values will be from -1.0 to 1.0, so this will shift them to
        // be from 0.0 to 2.0.
        noise_value += 1.0;

        // Update the noise range. This is used to record the minimum and
        // maximum noise values, which is important for later normalization.
        update_noise_range(noise_value);

        // Store the noise value in the map.
        map[z][y][x] = noise_value;
      }
    }
  }

  // Return the map.
  return map;
}

// Generates a river in the map and returns it to the generate_map function.
// This function uses simplex noise to generate the river.
private mixed *generate_river(mixed map) {
  int river_width = dimensions[WIDTH] / 6;
  int river_height = dimensions[HEIGHT];
  int river_start_x = dimensions[WIDTH] - (river_width * 3);
  int river_end_x = river_start_x + river_width;
  int river_start_y = 0;
  int center_y = dimensions[HEIGHT] / 2;
  mapping counts = ([
    "shallow water": 0,
    "water": 0,
    "deep water": 0,
  ]);

  // Generate the river.
  for(int y = 0; y < river_height; y++) {
    for(int x = 0; x < river_width; x++) {
      float river_noise_value = simplex2(to_float(x) / 10.0, to_float(y) / 20.0);
      float bias = (to_float(y) - center_y) * 0.05;  // Bias toward a more vertical flow
      float river_threshold = 1.0 + river_noise_value + bias;

      // The river has three levels of depth, shallow, regular, and deep and
      // is determined by the noise value.
      if(river_threshold < 1.0) {
        if(river_threshold < 0.5) {
          // Shallow river
          map[0][river_start_y + y][river_start_x + x] = -0.25;
          counts["shallow water"]++;
        } else if(river_threshold < 0.75) {
          // Regular river
          map[0][river_start_y + y][river_start_x + x] = -0.5;
          counts["water"]++;
        } else if(river_threshold < 1.0) {
          // Deep river
          map[0][river_start_y + y][river_start_x + x] = -1.0;
          counts["deep water"]++;
        }
      }
    }
  }

  // Now a pass to ensure that there are no deep water at the edges.
  // Make them regular water.
  for(int y = 0; y < river_height; y++) {
    int west_found = false, east_found = false;

    for(int x = 0; x < river_width; x++) {
      // Check the west side
      if(!west_found && map[0][river_start_y + y][river_start_x + x] == -1.0) {
        west_found = true;
        map[0][river_start_y + y][river_start_x + x] = -0.5;
        counts["deep water"]--;
        counts["water"]++;
      }

      // Check the east side
      if(!east_found && map[0][river_start_y + y][river_end_x - x] == -1.0) {
        east_found = true;
        map[0][river_start_y + y][river_end_x - x] = -0.5;
        counts["deep water"]--;
        counts["water"]++;
      }

      if(west_found && east_found)
        break;
    }

    west_found = east_found = false;
  }

  return map;
}

private void setup_wastes_shorts() {
  short_descriptions = ([
    "wastes" : ({
      "Barren Expanse",
      "Scorched Earth",
      "Desolate Stretch",
      "Rugged Terrain",
    }),
    "shallow water" : ({
      "Shallow Water",
    }),
    "water" : ({
      "River",
      "Water",
    }),
    "deep water" : ({
      "Deep Water",
      "Deep River",
    }),
  ]);
}

private void setup_wastes_longs() {
  long_descriptions = ([
    "wastes" : ({
      "The wasteland stretches before you, a desolate expanse of cracked "
      "earth and scattered debris. The air is dry and carries the faint scent "
      "of decay, with the sun beating down mercilessly.",

      "This area of the wasteland is littered with remnants of a forgotten "
      "past, rusted metal and broken glass glinting in the harsh light. The "
      "ground is uneven, with deep fissures and patches of thorny vegetation.",

      "A chilling wind sweeps across the wasteland, stirring up dust and "
      "debris. The landscape is dotted with skeletal remains of long-dead "
      "trees, their twisted branches reaching out like bony fingers.",

      "Silence reigns in this part of the wasteland, broken only by the "
      "occasional whistle of the wind. The ground is barren, with only the "
      "hardiest of plants struggling to survive in the harsh conditions."
    }),
    "shallow water" : ({
      "The shallow water glimmers under the sun, revealing a sandy bottom "
      "where small fish dart about.",

      "Gentle ripples disturb the surface of the shallow water, creating "
      "patterns that dance in the light.",

      "The water is cool and clear, inviting weary travelers to take a moment "
      "to refresh themselves.",

      "Surrounded by lush greenery, the shallow water reflects the vibrant "
      "colors of the flora around it."
    }),
    "water" : ({
      "The river flows steadily, its surface reflecting the sky above, "
      "while the sound of rushing water fills the air.",

      "This body of water is deep and wide, with currents that can be both "
      "calm and treacherous.",

      "The water is a rich blue, teeming with life beneath its surface, "
      "where fish swim and plants sway.",

      "Along the banks, the ground is soft and muddy, a testament to the "
      "power of the flowing water."
    }),
    "deep water" : ({
      "The deep water is dark and mysterious, hiding secrets beneath its "
      "surface that few have dared to uncover.",

      "A sense of foreboding surrounds the deep water, where shadows loom "
      "and the depths seem to stretch endlessly.",

      "The water here is cold and still, a stark contrast to the lively "
      "shallows nearby.",

      "Occasionally, the surface breaks as a large creature stirs below, "
      "reminding all of the dangers that lurk in the depths."
    }),
  ]);
}

public void setup_short(object room, string file) {
  int *coords = room->get_virtual_coordinates();
  string room_type = determine_room_type(coords...);

  switch(room_type) {
    case "deep water": case "water": case "shallow water":
      room->set_short(element_of(short_descriptions[room_type]));
      break;
    default:
      room->set_short(element_of(short_descriptions["wastes"]));
      break;
  }
}

public void setup_long(object room, string file) {
  int *coords = room->get_virtual_coordinates();
  string room_type = determine_room_type(coords...);

  switch(room_type) {
    case "deep water": case "water": case "shallow water":
      room->set_long(element_of(long_descriptions[room_type]));
      break;
    default:
      room->set_long(element_of(long_descriptions["wastes"]));
      break;
  }
}

// Setup the exits for the room. This is used to connect the rooms together.
// This is really not that complicated.
public void setup_exits(object room) {
  int *coords = room->get_virtual_coordinates();
  int x, y, z;
  int max_x, max_y;
  string current_room_type;
  string room_type;
  mapping exits;
  string maze_entrance_file, maze_exit_file;
  int *maze_entrance, *maze_exit;
  int *cavern_entrance, *cavern_exit;

  x = coords[2];
  y = coords[1];
  z = coords[0];

  max_x = get_map_width();
  max_y = get_map_height();

  current_room_type = determine_room_type(z, y, x);

  if(current_room_type == "impassable")
    return;

  // West
  if(x-1 >= 0) {
    room_type = determine_room_type(z, y, x-1);
    if(room_type != "impassable")
      room->add_exit("west", sprintf("%d,%d,%d", x-1, y, z));
  }

  // East
  if(x+1 < max_x) {
    room_type = determine_room_type(z, y, x+1);
    if(room_type != "impassable")
      room->add_exit("east", sprintf("%d,%d,%d", x+1, y, z));
  }

  // South
  if(y+1 < max_y) {
    room_type = determine_room_type(z, y+1, x);
    if(room_type != "impassable")
      room->add_exit("south", sprintf("%d,%d,%d", x, y+1, z));
  }

  // North
  if(y-1 >= 0) {
    room_type = determine_room_type(z, y-1, x);
    if(room_type != "impassable")
      room->add_exit("north", sprintf("%d,%d,%d", x, y-1, z));
  }

    // Northeast
  if(x+1 < max_x && y-1 >= 0) {
    room_type = determine_room_type(z, y-1, x+1);
    if(room_type != "impassable")
      room->add_exit("northeast", sprintf("%d,%d,%d", x+1, y-1, z));
  }

  // Northwest
  if(x-1 >= 0 && y-1 >= 0) {
    room_type = determine_room_type(z, y-1, x-1);
    if(room_type != "impassable")
      room->add_exit("northwest", sprintf("%d,%d,%d", x-1, y-1, z));
  }

  // Southeast
  if(x+1 < max_x && y+1 < max_y) {
    room_type = determine_room_type(z, y+1, x+1);
    if(room_type != "impassable")
      room->add_exit("southeast", sprintf("%d,%d,%d", x+1, y+1, z));
  }

  // Southwest
  if(x-1 >= 0 && y+1 < max_y) {
    room_type = determine_room_type(z, y+1, x-1);
    if(room_type != "impassable")
      room->add_exit("southwest", sprintf("%d,%d,%d", x-1, y+1, z));
  }

  // Forest entrance
  if(x == 0 && y == 0 && z == 0)
    room->add_exit("west", "../forest/27,11,0");

  // Setup the exit to the maze, if there is one.
  maze_entrance = "/d/maze/maze_daemon"->query_east_entrance();
  if(maze_entrance[0] == z && maze_entrance[1] == y && maze_entrance[2] == x) {
    maze_entrance_file = sprintf("%d,%d,%d",
      maze_entrance[2], maze_entrance[1], maze_entrance[0]);

    maze_exit = "/d/maze/maze_daemon"->query_east_exit();
    maze_exit_file = sprintf("%d,%d,%d", maze_exit[2], maze_exit[1], maze_exit[0]);

    room->add_exit("west", "/d/maze/"+maze_exit_file);
  }

  // Set the exit to the caverns
  // Do we have a valid exit to the caverns?
  cavern_entrance = "/d/cavern/cavern_daemon"->query_entrance();
  cavern_exit = "/d/cavern/cavern_daemon"->query_exit();
  if(z == cavern_entrance[0] && y == cavern_entrance[1] && x == cavern_entrance[2])
    room->add_exit("down",
      sprintf("../cavern/%d,%d,%d",
        cavern_exit[2], cavern_exit[1], cavern_exit[0]
      )
    );
}

private nosave string *terrain_types = ({ "rocky", "sandy", "impassable", });
private nosave mixed *thresholds = ({
  ({ 0.3, "impassable" }),   //  0 -  30%
  ({ 1.2, "sandy" }),        // 30 -  60%
  ({ 1.7, "rocky" }),        // 60 -  85%
  ({ 2.0, "impassable" }),   // 85 - 100%
});

// Determine the room type based on the noise value.
varargs string determine_room_type(int z, int y, int x) {
  int value;
  string room_type;

  value = get_room_type(z, y, x);
  if(nullp(value))
    return null;

  // First, let's check if the room is part of the river and set it up
  // accordingly and then return.
  if(value == -1.0)
    return "deep water";
  else if(value == -0.5)
    return "water";
  else if(value == -0.25)
    return "shallow water";
  else
    // If it's not part of the river, then we can determine the room type
    // based on the noise value.
    foreach(mixed threshold in thresholds) {
      int threshold_value = threshold[0];
      string terrain_type = threshold[1];

      if(value <= threshold_value) {
        room_type = terrain_type;
        break;
      }
    }

  return room_type;
}

// Setup the room type for the room. This is used to set the environment of the
// room.
public void setup_room_type(object room) {
  int *coords = room->get_virtual_coordinates();
  string room_type;
  room_type = determine_room_type(coords...);

  if(nullp(room_type))
    return;

  room->set_room_environment(room_type);
}

// Print the map
void print_map() {
  int y;
  mixed *map = query_map_data()[0];

  for(y = 0; y < sizeof(map); y++)
    print_row(y);
}

// Print a row of the map.
private nosave mapping colour_map = ([
  "impassable" : COLOUR_D->rgb_to_colour(0, 0, 0),
  "sandy" : COLOUR_D->rgb_to_colour(255, 223, 186),
  "rocky" : COLOUR_D->rgb_to_colour(139, 137, 137),
  "shallow water" : COLOUR_D->rgb_to_colour(40, 139, 184),
  "water" : COLOUR_D->rgb_to_colour(35, 90, 186),
  "deep water" : COLOUR_D->rgb_to_colour(24, 63, 130),
]);
private nosave mapping symbols_map = ([
  "impassable" : "#",
  "sandy" : ".",
  "rocky" : "^",
  "shallow water" : "~",
  "water" : "~",
  "deep water" : "~",
]);

void print_row(int y) {
  int x;
  mixed *map = query_map_data()[0][y];

  for(x = 0; x < sizeof(map); x++) {
    string terrain = determine_room_type(0, y, x);
    string symbol = symbols_map[terrain];
    string colour = sprintf("{{re1}}{{%'0'4d}}", colour_map[terrain]);
    tell_me(sprintf("%s%s{{res}}", colour, symbol));
  }

  tell_me("\n");
}
