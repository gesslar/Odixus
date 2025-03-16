/**
 * @file /d/village/village_path5.c
 * @description Path through the village
 *
 * @created 2024-08-12 - Gesslar
 * @last_modified 2024-08-12 - Gesslar
 *
 * @history
 * 2024-08-12 - Gesslar - Created
 */

inherit __DIR__ "village_base";

void setup() {
  set_short("A Path Through the Village");
  set_long(
  "West from the village square, a brick path leads to a quieter part of the "
  "village. The bricks, though less maintained, are in good condition with "
  "occasional weeds peeking through. As you walk, the bustling sounds fade, "
  "replaced by a sense of community. To the north stands a dilapidated "
  "structure, its worn exterior contrasting with the welcoming atmosphere "
  "and activity around it. The path, free of dwellings, emphasizes this lone "
  "building's significance. The air carries scents of old wood, fresh ink, "
  "and excitement, hinting at the building's role as a gathering place for "
  "newcomers to the village and beyond.");

  set_exits(([
    "north": "centre",
    "east" : "village_path2",
    "south": "../forest/2,0,0",
  ]));

  set_items(([
    ({ "village square", "square" }) :
      "The bustling centre of the village, now fading into the distance "
      "as you move westward. Its lively atmosphere gives way to the "
      "quieter, more residential areas ahead.",
    ({ "brick path", "path" }) :
      "A path of weathered bricks stretches before you, showing signs "
      "of age but still well-maintained. Occasional weeds peek through "
      "the cracks, adding a touch of wild charm to the route.",
    ({ "dilapidated structure", "structure" }) :
      "To the north, a somewhat dilapidated structure stands out among "
      "the surroundings. Its weathered appearance suggests years of "
      "neglect, yet it retains a certain mysterious charm.",
    "gardens" :
      "Glimpses of small gardens can be seen near the dwellings, "
      "contributing to the earthy scents in the air and adding splashes "
      "of colour to the residential landscape.",
    ({ "weeds", "cracks" }) :
      "Small weeds occasionally sprout between the bricks, their "
      "presence a testament to the less rigorous maintenance of this "
      "quieter part of the village.",
    "air" :
      "The air here carries a mix of domestic scents - freshly baked "
      "bread, laundry drying in the breeze, and the earthy smell of "
      "nearby gardens, creating a homely atmosphere.",
  ]));
}
