/**
 * @file /d/forest/zone.c
 * @description The zone object for the forest.
 *
 * @created 2024-08-21 - Gesslar
 * @last_modified 2024-08-21 - Gesslar
 *
 * @history
 * 2024-08-21 - Gesslar - Created
 */


inherit STD_VIRTUAL_SERVER ;

private void load_forest_map() ;

private mapping forest_map = ([]) ;

void setup() {
    load_forest_map() ;
}

private void load_forest_map() {
    string *lines = explode(read_file(__DIR__ "forest_map.txt"), "\n") ;
    int y = 0, z = 0 ;
    for(int i = 0; i < sizeof(lines); i += 2) {  // Skip every other line
        string line = lines[i] ;
        int x = 0 ;
        for(int j = 0; j < sizeof(line); j += 2) {  // Skip every other character
            string char = line[j..j] ;
            if(char == "O" || char == "X") {
                forest_map[sprintf("%d,%d,%d", x, y, z)] = char ;
            }
            x++ ;
        }
        y++ ;
    }
}

mapping get_forest_map() {
    return forest_map ;
}

object generate_object(string file) {
    string *parts ;
    int x, y, z ;

    parts = dir_file(file) ;

    if(sscanf(parts[1], "%d,%d,%d", x, y, z) != 3)
        return 0 ;

    if(!forest_map[parts[1]])
        return 0 ;

    return new(__DIR__ "forest_base", parts[1]) ;
}
