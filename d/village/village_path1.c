/**
 * @file /d/village/village_path1.c
 * @description Path through the village
 *
 * @created 2024-02-04 - Gesslar
 * @last_modified 2024-02-04 - Gesslar
 *
 * @history
 * 2024-02-04 - Gesslar - Created
 */

inherit __DIR__ "village_inherit" ;

void setup() {
    set_short("A Path Through the Village");
    set_long(
"Just north of the village square, a cobblestone path stretches out, leading "
"past a series of quaint village establishments. Along this path, the unassuming "
"facade of the local baker's shop stands out, marked by a simple sign featuring "
"a painted loaf of bread. The aroma of fresh baking subtly infuses the air "
"around this stretch, guiding locals and travelers alike towards the promise of "
"wholesome, everyday fare. The path itself, worn by the footsteps of countless "
"patrons, speaks to the steady rhythm of village life, where the baker's humble "
"offerings are a staple of daily existence.") ;
    set_exits(([
        "south": "square",
        "west" : "baker",
        "east" : "shop",
        "north": "field/field1",
    ])) ;
    set_items(([
        ({ "village square", "square" }) :
            "The village square lies to the south, a bustling hub of activity "
            "and the heart of community gatherings. It's where paths cross, "
            "both literally and metaphorically, connecting all facets of "
            "village life.",
        ({ "cobblestone path", "path" }) :
            "This path, paved with rounded cobblestones, leads adventurers "
            "from the village square towards the north. Each stone, smoothed "
            "over by years of travel, contributes to the path's charm, "
            "inviting exploration beyond the town's central hustle.",
        ({ "baker's shop", "shop" }) :
            "Nestled among village establishments, the baker's shop draws "
            "the eye with its unpretentious facade. A beacon of comfort, its "
            "presence is a reminder of the simple pleasures that bind the "
            "community together.",
        ({ "simple sign", "sign" }) :
            "A modest wooden sign hangs outside the bakery, its loaf of bread "
            "painted with a rustic touch. It's a symbol of the shop's "
            "dedication to the fundamental joy of baking, offering a warm "
            "welcome to all who pass by.",
    ])) ;
}
