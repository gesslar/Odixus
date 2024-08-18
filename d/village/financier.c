/**
 * @file /d/village/financier.c
 * @description Financier's office
 *
 * @created 2024-02-29 - Gesslar
 * @last_modified 2024-02-29 - Gesslar
 *
 * @history
 * 2024-02-29 - Gesslar - Created
 */

inherit __DIR__ "village_inherit" ;
inherit M_BANK ;

void setup() {
    set_short("Well-appointed office") ;
    set_long(
"The financier's office, marked by a polished brass plaque, exudes quiet "
"elegance. Dark wood paneling and shelves of ledgers convey reliability, "
"while a large oak desk centers the room, ready for business. Here, in an "
"atmosphere of calm efficiency, the financier oversees the village's wealth, "
"offering counsel and loans with a steady hand. This space is the heart of "
"financial dealings, where futures are shaped with care and precision.") ;

    set_items(([
    ({ "polished brass plaque", "brass plaque", "polished plaque", "plaque" }) :
        "This gleaming plaque beside the door serves as a beacon of trust and "
        "professionalism. Its polished surface reflects the financier's "
        "commitment to precision and integrity in all transactions.",
    ({ "dark wood paneling", "wood paneling", "paneling" }) :
        "The walls, adorned with dark wood paneling, exude warmth and "
        "solidity, creating an atmosphere of stability. Each panel tells a "
        "story of meticulous care and traditional values, grounding the "
        "office in a sense of enduring reliability.",
    ({ "leather-bound ledgers", "ledgers" }) :
        "Rows of leather-bound ledgers sit on the shelves, their spines a "
        "testament to the meticulous record-keeping within. The leather, aged "
        "and supple, holds the weight of countless transactions, embodying "
        "the history and diligence of the financier's practice.",
    ({ "oak desk", "desk" }) :
        "Dominating the room, the large oak desk stands as the centerpiece of "
        "the financier's domain. Its broad surface, polished to a soft glow, "
        "is a workspace where fortunes are shaped and futures secured, "
        "symbolizing the financier's role as a pillar of the community.",
    ({ "quills and ink wells", "quills", "ink wells", "wells" }) :
        "Quills and inkwells, neatly arranged on the desk, are ready tools of "
        "the financier's trade. Each quill, poised for action, partners with "
        "the ink to document the ebb and flow of wealth, marking commitments "
        "with a flourish that binds word to deed.",
    ])) ;

    add_exit("west", "village_path3") ;

    init_bank() ;
}
