// /d/village/village_path1.c
//
// Created:     2024/02/04: Gesslar
// Last Change: 2024/02/04: Gesslar
//
// 2024/02/04: Gesslar - Created

inherit __DIR__ "village_inherit" ;

void setup() {
    set_short("A Path Through the Village");
    set_long(
"Eastward from the lively center of the village square, the path becomes more "
"refined, with well-placed stones and borders of flowering shrubs indicating a "
"transition to a more affluent section of the village. Here, among well-tended "
"gardens and impressive facades, stands a discreet yet elegant building known to "
"house the village financier. A polished brass plaque beside the door gleams in "
"the sunlight, marking the entrance with a level of sophistication that speaks "
"to the serious nature of the transactions within. This path, often traversed by "
"those seeking counsel or investment, leads directly to the financier's door, "
"promising discretion and expertise for those navigating the complexities of "
"wealth and commerce.") ;
    set_exits(([
        "east" : "financier",
        "west" : "square",
    ]) ) ;
    set_items(([
        ({ "village square", "square" }) :
            "The bustling heart from which the path departs, leaving behind the "
            "communal vibrancy for a realm of quiet prosperity. It's a place "
            "where the village's pulse can be felt, now giving way to the serene "
            "elegance of the affluent section.",
        ({ "refined path", "path" }) :
            "With its precisely laid stones and the delicate framing of "
            "flowering shrubs, this path signifies a journey into "
            "sophistication. It leads one away from the rustic charm of the "
            "village square to a world of manicured beauty and grace.",
        ({ "well-tended gardens", "gardens", "garden" }) :
            "Each garden along the path is a testament to meticulous care and "
            "aesthetic pride, showcasing a riot of colours and scents. These "
            "gardens are the village's jewels, reflecting the prosperity and "
            "refined tastes of their caretakers.",
        ({ "impressive fcades", "facdes" }) :
            "The facades here are not just structures; they are statements of "
            "status and achievement. With their grand designs and impeccable "
            "maintenance, they stand as silent yet eloquent testimonies to the "
            "village's wealthier denizens.",
        ({ "financier's building", "building" }) :
            "This building, discreet in its elegance, commands respect. Its "
            "polished brass plaque shines as a beacon of professionalism and "
            "trust, inviting those who seek financial wisdom and partnership "
            "within its esteemed walls.",
        ({ "polished brass plaque", "brass plaque", "polished plaque", "plaque" }) :
            "Nestled beside the door, the plaque is not just an identifier but "
            "a promise—a promise of confidentiality, expertise, and the serious "
            "business of wealth management that defines the very essence of the "
            "financier's practice.",
    ])) ;
}
