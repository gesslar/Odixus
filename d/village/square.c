/**
 * @file /d/village/square.c
 * @description Square of the village of Olum
 *
 * @created 2024-02-04 - Gesslar
 * @last_modified 2024-02-04 - Gesslar
 *
 * @history
 * 2024-02-04 - Gesslar - Created
 */

inherit __DIR__ "village_base" ;

void setup() {
    set_short("Village Square of Olum");
    set_long(
"At the village center lies a bustling square, surrounded by ancient, timber-"
"framed buildings. Cobblestone paths lead to a moss-covered stone well, the "
"communal heart where market stalls flaunt local produce and crafts. Laughter "
"and conversation ripple through the air as elders on benches share stories, "
"overseeing the vibrant tableau of daily life. The well stands as a testament "
"to the village's enduring spirit, anchoring the square's lively exchange "
"and community bonds.") ;

    set_exits(([
        "north": "village_path1",
        "west" : "village_path2",
        "east" : "village_path3",
        "south": "village_path4",
        "down" : "../tunnels/0,0,-1",
    ])) ;

    set_room_size(({2,2,1})) ;

    set_items(([
        ({ "timber-framed buildings", "buildings", "ancient buildings" }) :
            "The buildings surrounding the square are a testament to the "
            "village's history. Their timber frames, darkened with age, "
            "stand sturdy and proud, each beam and post telling a story of "
            "craftsmanship and endurance through the years.",
        ({ "cobblestone paths", "paths", "cobblestones" }) :
            "Worn smooth by countless footsteps, the cobblestone paths "
            "weave through the square, guiding villagers and visitors alike. "
            "Each stone, unique in shape and colour, fits snugly against its "
            "neighbors, creating a durable and charming surface.",
        ({ "moss-covered stone well", "well", "stone well" }) :
            "At the heart of the square stands the village well, its stones "
            "partially covered in a soft blanket of moss. It serves not just "
            "as a water source, but as a gathering point for the community, "
            "its presence a constant in the ebb and flow of village life.",
        ({ "market stalls", "stalls" }) :
            "Colorful awnings and wooden counters mark the market stalls, "
            "where local vendors display their wares. From fresh produce to "
            "handcrafted goods, these stalls are a vibrant showcase of the "
            "village's productivity and creativity.",
        ({ "elders", "benches" }) :
            "Scattered around the square are benches where the village elders "
            "often sit. These wise individuals, with their weathered faces "
            "and kind eyes, watch over the daily activities, always ready to "
            "share a story or offer advice to those who seek it.",
    ])) ;
}
