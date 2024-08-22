/**
 * @file /std/room/terrain.c
 * @description Terrain types
 *
 * @created 2024-08-19 - Gesslar
 * @last_modified 2024-08-19 - Gesslar
 *
 * @history
 * 2024-08-19 - Gesslar - Created
 */

private nosave string *_terrain_types = ({
    "city",
    "road",
    "indoor",
    "outdoor",
    "forest",
    "grass",
    "ocean",
    "plains",
    "swamp",
}) ;

private nosave string _terrain ;

void set_terrain(string terrain) {
    if(!of(terrain, _terrain_types))
        return ;

    _terrain = terrain ;
}

string query_terrain() {
    return _terrain ;
}
