/**
 * @file /d/village/shop.c
 * @description A basic shop in Olum village.
 *
 * @created 2024-08-01 - Gesslar
 * @last_modified 2024-08-01 - Gesslar
 *
 * @history
 * 2024-08-01 - Gesslar - Created
 */

inherit __DIR__ "village_base" ;

inherit M_SHOP ;

void setup() {
    set_short("Olum Village Shop");
    set_long(
    "The small, rustic shop, marked by a weathered wooden sign, emanates a "
    "cozy charm. Rough-hewn shelves made of dark oak are packed with a motley "
    "assortment of goods, from simple tools to peculiar trinkets. A sturdy "
    "wooden counter, worn smooth by years of use, sits near the entrance, "
    "where the friendly shopkeeper greets visitors with a warm smile. The scent "
    "of old wood and herbs fills the air, adding to the inviting, homely "
    "atmosphere. Here, amidst the humble surroundings, one can find everyday "
    "necessities and rare finds, each item with a story waiting to be "
    "discovered.");

    set_exits(([
        "west" : "village_path1",
    ])) ;

    set_items(([
        ({ "weathered wooden sign", "wooden sign", "sign" }) :
            "The sign hanging outside the shop is a testament to its "
            "longevity. Though the paint is faded and the wood shows signs "
            "of age, it still proudly displays the shop's name, welcoming "
            "visitors with rustic charm.",
        ({ "rough-hewn shelves", "shelves", "dark oak shelves" }) :
            "These shelves, crafted from sturdy dark oak, line the walls of "
            "the shop. Despite their rough appearance, they hold a wide array "
            "of goods with surprising elegance, each item finding its perfect "
            "place among the knots and grains of the wood.",
        ({ "wooden counter", "counter" }) :
            "Near the entrance stands a solid wooden counter, its surface "
            "polished smooth by countless transactions and friendly "
            "conversations. It serves as both a barrier and a bridge between "
            "the shopkeeper and customers, facilitating the exchange of "
            "goods, coin, and local gossip.",
        "shopkeeper" :
            "The shopkeeper, a fixture of the village as much as the shop "
            "itself, greets you with a warm smile. Their eyes twinkle with a "
            "mix of wisdom and mischief, hinting at the stories they could "
            "tell about the items in their care and the customers who've "
            "passed through over the years.",
        ({ "goods", "items", "trinkets" }) :
            "The shelves are packed with an eclectic mix of items. From "
            "practical tools for everyday use to curious trinkets that catch "
            "the eye, each piece seems to have a history of its own. Some "
            "gleam with newness, while others bear the patina of age and use.",
    ])) ;

    init_shop() ;

    add_shop_inventory(
        "/obj/weapon/piercing/rusty_sword",
        "/obj/armour/torso/leather_jerkin",
    ) ;

    set_terrain("indoor") ;
    set_room_type("shop") ;
}
