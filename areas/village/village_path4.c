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
"South from the village square, a meticulously laid brick path veers into a "
"realm of conspicuous opulence. Lined with extravagantly pruned hedges and "
"interspersed with statues of dubious taste, this route unmistakably leads to an "
"overly ostentatious manor. The manor, a sprawling edifice with gaudy "
"embellishments and towering spires, seems almost to compete with the sun for "
"brilliance. Gold leaf accents catch the light, and flamboyant banners flutter "
"from the rooftop, proclaiming the wealth (and perhaps the questionable taste) "
"of the occupants. This path, distinctly less worn than those leading to more "
"modest dwellings, guides visitors to a world of excess, where the line between "
"affluence and extravagance is not just crossed but obliterated.") ;
    set_exits(([
        "south" : "manor",
        "north" : "square",
    ]) ) ;
    set_items(([
        ({ "village square", "square" }) :
            "The starting point of many journeys within the village, the "
            "square's communal harmony fades into the background as one ventures "
            "south. It's a place of simple joys and unity, contrasting sharply "
            "with the luxury that lies ahead.",
        ({ "brick path", "path" }) :
            "This path, with its bricks laid with precision, serves as a grand "
            "avenue towards opulence. Each brick, a testament to meticulous "
            "design, guides the way to a display of wealth that knows no bounds, "
            "setting the stage for the grandeur to come.",
        ({ "pruned hedges", "hedges" }) :
            "The hedges along this path are not merely trimmed but sculpted, "
            "each one an exercise in extravagance. Their elaborate shapes speak "
            "of a desire to impress and dominate, framing the route with a "
            "greenery that's as controlled as it is lavish.",
        ({ "statues" }) :
            "Dotting the landscape, these statues vary from the grandiose to "
            "the perplexing, their artistic merit often overshadowed by their "
            "ostentation. They stand as silent sentinels to extravagance, each "
            "one a declaration of wealth that verges on the excessive.",
        ({ "ostentatious manor", "manor" }) :
            "Dominating the end of the path, the manor looms large, a spectacle "
            "of architectural excess. Its spires reach skyward, while gold leaf "
            "and flamboyant banners vie for attention, embodying a wealth so "
            "immense it borders on the surreal.",
        ({ "gold leaf accents", "accents" }) :
            "These shimmering details capture the sunlight, turning the manor "
            "into a beacon of affluence. They adorn the exterior with a "
            "brilliance that's both awe-inspiring and indicative of the "
            "occupants' desire to showcase their unmatched wealth.",
        ({ "flamboyant bnners", "banners" }) :
            "Fluttering from the rooftop, these banners are as bold in colour as "
            "they are in proclamation. They are the manor's flags, waving "
            "declarations of a prosperity so profound that it eclipses modesty, "
            "heralding the power and prestige of those within.",
    ])) ;
}
