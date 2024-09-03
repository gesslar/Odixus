/**
 * @file /d/village/bakery.c
 * @description A cozy bakery in Olum village.
 *
 * @created 2024-08-01 - Gesslar
 * @last_modified 2024-08-01 - Gesslar
 *
 * @history
 * 2024-08-01 - Gesslar - Created
 */

inherit __DIR__ "village_base" ;

inherit M_SHOP_MENU ;

void setup() {
    set_short("Olum Village Bakery");
    set_long(
    "The cozy bakery, marked by a cheerful wooden sign shaped like a "
    "loaf of bread, fills the air with enticing aromas. Wooden shelves "
    "behind a glass display case are laden with an array of freshly "
    "baked goods, from crusty loaves to sweet pastries. A large stone "
    "oven dominates one wall, radiating warmth throughout the shop. "
    "The friendly baker, dusted with flour, kneads dough at a worn "
    "wooden worktable. The scent of yeast, sugar, and warm bread "
    "creates an irresistible atmosphere that welcomes visitors and "
    "tempts them with promises of delicious treats.");

    set_exits(([
        "east" : "village_path1",
    ])) ;

    set_items(([
        ({ "wooden sign", "sign", "bread-shaped sign" }) :
            "A charming wooden sign, carved in the shape of a plump loaf of "
            "bread, hangs above the bakery entrance. The words 'Olum Village "
            "Bakery' are painted in warm, inviting colors, beckoning "
            "passersby with the promise of fresh baked goods.",
        ({ "glass display case", "display case", "case" }) :
            "A spotless glass display case stretches along one side of the "
            "bakery, showcasing an array of tempting baked goods. Crusty "
            "breads, flaky pastries, and colorful cakes are artfully "
            "arranged, each item looking more delicious than the last.",
        ({ "wooden shelves", "shelves" }) :
            "Sturdy wooden shelves line the wall behind the display case, "
            "holding additional loaves of bread and boxes of specialty items. "
            "The rich, dark wood complements the golden hues of the baked "
            "goods, creating a warm and appetizing display.",
        ({ "stone oven", "oven" }) :
            "A large stone oven dominates one wall of the bakery, its arched "
            "opening occasionally revealing the dancing flames within. The "
            "oven radiates a comforting warmth and is clearly the heart of "
            "this establishment, responsible for the mouthwatering aromas "
            "that fill the air.",
        "baker" :
            "The village baker, a jolly figure with rosy cheeks and a flour-"
            "dusted apron, works diligently at the worktable. Their skilled "
            "hands knead dough with practiced ease, pausing occasionally to "
            "greet customers with a warm smile and friendly word.",
        ({ "wooden worktable", "worktable", "table" }) :
            "A sturdy wooden worktable stands in the center of the bakery, "
            "its surface dusted with flour and bearing the marks of years of "
            "use. This is where the magic happens, as the baker transforms "
            "simple ingredients into delicious breads and pastries.",
        ({ "baked goods", "goods", "breads", "pastries" }) :
            "An enticing array of baked goods fills the bakery. Golden loaves "
            "of various shapes and sizes, some studded with nuts or dried "
            "fruits, sit alongside delicate pastries glistening with sugar or "
            "filled with colorful jams. The variety is impressive, catering "
            "to every taste and occasion.",
    ])) ;

    init_shop() ;

    add_menu_item("food", "/obj/food/muffin") ;
    add_menu_item("drink", "/obj/drink/strawberry_juice") ;

    set_terrain("indoor") ;
    set_room_type("food") ;
}
