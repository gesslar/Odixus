// /areas/village/village_path1.c
//
// Created:     2024/02/04: Gesslar
// Last Change: 2024/02/04: Gesslar
//
// 2024/02/04: Gesslar - Created

inherit __DIR__ "village_inherit" ;

void setup() {
    set_short("A Path Through the Village");
    set_long(
"Westward from the village square, the path transitions from cobblestone to "
"packed earth, guiding travelers towards the quieter side of the village. Here, "
"nestled among the modest dwellings, a small, unassuming tavern quietly asserts "
"its presence. Its wooden facade, weathered by time, and a dimly lit lantern by "
"the door, offer a welcoming if understated, invitation. The sign, swaying "
"gently, depicts a frothy mug, symbolizing the simple comforts waiting within. "
"This stretch of the path, less traveled yet worn smooth by the tavern's "
"regulars, leads to a place of rest and camaraderie, where tales and ale are "
"shared with equal generosity.") ;
    set_exits(([
        "east" : "square",
        "west" : "tavern",
    ]) ) ;
    set_items(([
        ({ "village square", "square" }) :
            "The bustling epicenter from which this path begins, fading into "
            "the distance as one moves westward. It's a reminder of the "
            "village's vibrancy, now giving way to the tranquil outskirts.",
        ({ "packed earth path", "path" }) :
            "Shifting from the structured cobblestone, this path of packed "
            "earth offers a softer journey underfoot, leading adventurers "
            "towards serenity. It's a road less traveled, but rich in the "
            "promise of quiet discovery.",
        "tavern" :
            "A small haven of warmth and welcome, the tavern's modest size "
            "belies the depth of solace found within. Its weathered wooden "
            "facade, a testament to many a story shared and heard, invites all "
            "to enter and leave the world behind.",
        ({ "wooden facade", "facade" }) :
            "Bearing the marks of time, the facade stands as a humble guardian "
            "of the tavern's threshold. It speaks of endurance, of countless "
            "greetings and farewells, and of the simple, unspoken promise of "
            "shelter to those who seek it.",
        ({ "dimly lit lantern", "lantern" }) :
            "Hanging by the door, the lantern casts a soft glow, piercing the "
            "twilight to guide the wayfarer. It's a beacon of the familiar and "
            "the cozy, an understated herald of the refuge that lies beyond the "
            "tavern's door.",
        ({ "swaying sign", "sign" }) :
            "With each gentle sway, the sign and its frothy mug emblem beckon "
            "to the thirsty and the weary. It symbolizes the universal language "
            "of rest and refreshment, offering a silent but heartfelt invitation "
            "to come and partake.",
    ])) ;
}
