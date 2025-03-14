/**
 * @file /d/village/centre.c
 * @description The commmunity centre
 *
 * @created 2024-08-12 - Gesslar
 * @last_modified 2024-08-12 - Gesslar
 *
 * @history
 * 2024-08-12 - Gesslar - Created
 */

#include <classes.h>

inherit __DIR__ "village_base";
inherit STD_STORAGE_ROOM;

inherit CLASS_STORAGE;

void setup() {
    set_light(1);
    set_short("The Gathering Place");
    set_long(
        "Stepping into this weathered structure, you're immediately "
        "enveloped by a cacophony of voices and laughter. The large, open "
        "room bears the marks of countless gatherings, its wooden floors "
        "scuffed and walls adorned with faded notices and hand-drawn "
        "maps. Mismatched chairs and tables are scattered about, some "
        "clustered in intimate groups, others pushed aside clear space. "
        "A rickety bulletin board near the entrance is plastered with "
        "announcements and requests, while a well-worn bar in the corner "
        "serves as a makeshift information desk. Despite its rundown "
        "appearance, the place buzzes with energy as villagers and "
        "visitors alike mingle, sharing stories and forging connections in "
        "this humble yet vibrant hub of community life."
    );

    set_exits(([
        "south": "village_path5",
        "west" : "lockers",
    ]));

    set_items(([
        ({ "wooden floors", "floors" }) :
            "The wooden floors, though scuffed and worn, speak of countless "
            "feet that have trod here, each mark a testament to the lively "
            "gatherings this place has hosted.",
        ({ "faded notices", "hand-drawn maps", "notices", "maps" }) :
            "The walls are adorned with an eclectic mix of faded notices and "
            "hand-drawn maps, offering glimpses into local history and "
            "upcoming events. Some corners are peeling, but the information "
            "remains eagerly consumed by curious eyes.",
        ({ "mismatched chairs", "tables", "chairs" }) :
            "An assortment of chairs and tables, no two quite alike, are "
            "scattered throughout the room. Their arrangement seems to shift "
            "constantly, adapting to the needs of the moment and the groups "
            "that gather here.",
        ({ "rickety bulletin board", "bulletin board", "board" }) :
            "Near the entrance, a bulletin board leans slightly, its surface "
            "barely visible beneath layers of pinned papers. Job postings, "
            "lost pet notices, and community announcements compete for space "
            "in this analog social network.",
        ({ "well-worn bar", "bar", "information desk" }) :
            "The bar in the corner, once perhaps a place for drinks, now "
            "serves as an informal information desk. Its surface is marked "
            "with rings from countless mugs, and behind it, shelves hold "
            "stacks of pamphlets and local guidebooks.",
        "people" :
            "A diverse mix of villagers and visitors fill the space, their "
            "animated conversations and warm laughter creating an atmosphere "
            "of welcome and community. Some huddle in deep discussion, while "
            "others move about, greeting friends old and new.",
    ]));

    set_storage_options(new(class StorageOptions,
        storage_type: "public",
        storage_org: "community_centre",
        clean_on_empty: true,
        restore_on_load: true
    ));

    init_storage_room();
    set_terrain("indoor");
    set_room_type("inn");
}
