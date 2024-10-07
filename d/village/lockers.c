/**
 * @file /d/village/lockers.c
 * @description A place for villagers to store their personal belongings.
 *
 * @created 2024-08-13 - Gesslar
 * @last_modified 2024-08-13 - Gesslar
 *
 * @history
 * 2024-08-13 - Gesslar - Created
 */

#include <classes.h>

inherit __DIR__ "village_base" ;
inherit STD_STORAGE_ROOM ;

inherit CLASS_STORAGE ;

string storage_id() ;

void setup() {
    set_light(1) ;
    set_short("Village Storage - Private Lockers") ;
    set_long(
    "This room, an extension of the village's central storage area, "
    "is dedicated to private lockers. The space maintains the same "
    "rundown appearance as the main storage room to the east, with "
    "cracked plaster walls and a worn wooden floor. Instead of shelves, "
    "however, this area is lined with weathered trunks of various sizes. "
    "Each trunk bears the name of its owner, etched or painted on its "
    "surface. Despite their aged appearance, these trunks seem sturdy "
    "and secure, providing villagers with a place to store their personal "
    "belongings. The air is thick with the scent of old wood and leather, "
    "adding to the room's rustic charm."
    ) ;

    set_exits(([
        "east" : "/d/village/centre"
    ])) ;

    set_items(([
        ({ "walls", "plaster", "plaster walls" }) :
            "The walls are covered in cracked and peeling plaster, revealing "
            "glimpses of the rough stone beneath. Years of neglect have left "
            "their mark, but the structure remains solid.",
        ({ "floor", "wooden floor" }) :
            "The wooden floor is worn smooth by countless footsteps. Some "
            "planks creak underfoot, while others have been patched or "
            "replaced over time.",
        ({ "trunks", "lockers", "weathered trunks" }) :
            "Lining the walls are numerous weathered trunks of various sizes. "
            "Each one bears the name of its owner, either etched into the "
            "wood or painted on the surface. Despite their aged appearance, "
            "they look sturdy and well-maintained.",
        ({ "names", "owner names" }) :
            "Each trunk bears a unique name, identifying its owner. Some "
            "are neatly painted, others roughly carved, creating a patchwork "
            "of personal touches in this communal space."
    ])) ;

    set_storage_options(new(class StorageOptions,
        storage_type: "private",
        storage_org: "olum",
        storage_id: (: storage_id :),
        clean_on_empty: true,
        restore_on_load: true
    )) ;

    init_storage_room() ;

    set_terrain("indoor") ;
    set_room_type("storage") ;
}

string storage_id() { return query_privs(this_body()) ; }
