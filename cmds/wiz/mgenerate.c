/**
 * @file /cmds/wiz/mgenerate.c
 * @description Generates an area map file.
 *
 * @created 2024/07/24 - Gesslar
 * @last_modified 2024/07/24 - Gesslar
 *
 * @history
 * 2024/07/24 - Gesslar - Created
 */

inherit STD_CMD ;
inherit M_LOG ;

private string generate_cell(string area_type, int x, int y, int width, int ref *elevation_map, int ref terrain_seed) ;
string generate_area(string area_name, string area_type, int width, int height) ;
private void generate_elevation_map(int width, int height, int seed, int ref *elevation_map) ;
private void smooth_elevation_map(int width, int height, int ref *elevation_map) ;

private nosave int MAX_WIDTH = 100 ;
private nosave int MAX_HEIGHT = 100 ;

private nosave string *valid_area_types = ({ "forest", "mountain", "desert" });

mixed main(object tp, string str) {
    string aname, atype ;
    int width, height ;
    string result ;
    string path ;

    if(!str)
        return "Syntax: mgenerate <area name> <area type> <width> <height>\n";

    if(sscanf(str, "%s %s %d %d", aname, atype, width, height) != 4)
        return "Syntax: mgenerate <area name> <area type> <width> <height>\n";


    _info(tp, "Generating area: " + aname + " of type: " + atype + " with dimensions: " + width + "x" + height);

    result = generate_area(aname, atype, width, height);

    path = user_path(tp) + aname + ".map";

    write_file(path, result, 1) ;

    return "Area map file generated: " + path + "\n";
}

private string generate_terrain(string area_type, int seed) {
    int *result;
    int roll;

    result = prandom(seed, 100);
    roll = result[1];

    _log("Area type: " + area_type + ", Roll: " + roll);  // Add this line for debugging

    switch(area_type) {
        case "forest":
            return (roll < 80) ? "F" : "C"; // 80% forest, 20% clearing
        case "mountain":
            return (roll < 70) ? "M" : "H"; // 70% mountain, 30% hill
        case "desert":
            return (roll < 90) ? "S" : "O"; // 90% sand, 10% oasis
        default:
            return "P"; // Plain as default
    }
}

string generate_area(string area_name, string area_type, int width, int height) {
    string *map;
    int y, x, seed, terrain_seed;
    string line;
    string cell;
    int *elevation_map;

    if(width > MAX_WIDTH || height > MAX_HEIGHT) {
        error("Area dimensions exceed maximum allowed size.");
    }

    map = allocate(height);
    seed = time_ns();
    terrain_seed = seed;  // Use a separate seed for terrain generation
    elevation_map = allocate(height * width);

    generate_elevation_map(width, height, seed, ref elevation_map);
    smooth_elevation_map(width, height, ref elevation_map);

    for(y = 0; y < height; y++) {
        line = "";
        for(x = 0; x < width; x++) {
            cell = generate_cell(area_type, x, y, width, ref elevation_map, ref terrain_seed);
            line += cell + " ";
        }
        map[y] = line[0..<2];  // Remove trailing space
    }

    return implode(map, "\n");
}

private string generate_cell(string area_type, int x, int y, int width, int ref *elevation_map, int ref terrain_seed) {
    string terrain, elevation;
    int *result;

    result = prandom(terrain_seed, MAX_INT);
    terrain_seed = result[0];  // Update the terrain seed
    terrain = generate_terrain(area_type, result[1]);

    elevation = sprintf("%d", elevation_map[y * width + x]);

    return terrain + elevation + " ";  // Note the blank space for the feature column
}

private void generate_elevation_map(int width, int height, int seed, int ref *elevation_map) {
    int y, x;
    int *result;

    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            result = prandom_range(seed, 0, 4);
            seed = result[0];
            elevation_map[y * width + x] = result[1];
        }
    }
}

private void smooth_elevation_map(int width, int height, int ref *elevation_map) {
    int y, x, ny, nx, sum, count;
    int *new_map;

    new_map = allocate(height * width);
    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            sum = elevation_map[y * width + x];
            count = 1;
            for(ny = y-1; ny <= y+1; ny++) {
                for(nx = x-1; nx <= x+1; nx++) {
                    if(ny >= 0 && ny < height && nx >= 0 && nx < width && (ny != y || nx != x)) {
                        if(abs(elevation_map[ny * width + nx] - elevation_map[y * width + x]) <= 1) {
                            sum += elevation_map[ny * width + nx];
                            count++;
                        } else {
                            sum += elevation_map[y * width + x] + (elevation_map[ny * width + nx] > elevation_map[y * width + x] ? 1 : -1);
                            count++;
                        }
                    }
                }
            }
            new_map[y * width + x] = sum / count;
        }
    }
    elevation_map = new_map;
}
