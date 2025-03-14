/**
 * @file /d/cavern/zone.c
 * @description Zone for the caverns
 *
 * @created 2024-09-18 - Gesslar
 * @last_modified 2024-09-18 - Gesslar
 *
 * @history
 * 2024-09-18 - Gesslar - Created
 */

inherit STD_VIRTUAL_SERVER;

object generate_object(string file) {
    object result;

    if(pcre_match(file, "^\\d+,\\d+,-?\\d+$")) {
        result = new(__DIR__ "cavern_base", file);
        return result;
    }

    return 0;
}
