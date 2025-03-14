/**
 * @file /d/village/house/hall3.c
 * @description The third hall in the manor.
 *
 * @created 2024-09-13 - Gesslar
 * @last_modified 2024-09-13 - Gesslar
 *
 * @history
 * 2024-09-13 - Gesslar - Created
 */

inherit __DIR__ "manor";

void setup() {
  set_short("The third hall");
  set_long("This hall is a grand corridor lined with extravagant paintings "
  "and ornate sconces. The floor is covered in a plush carpet that seems to "
  "swallow your footsteps. Each wall is adorned with mismatched frames, "
  "showcasing portraits of the residents in various poses of opulence. "
  "The air is thick with the scent of expensive candles, creating an "
  "atmosphere that feels both luxurious and overwhelming.");

  set_exits(([
    "north" : "hall2",
    "up" : "landing",
  ]));

  set_items(([
    "paintings" :
    "The extravagant paintings depict various scenes of opulence, each framed "
    "in ornate gold.",
    "sconces" :
    "The ornate sconces flicker with candlelight, casting a warm glow "
    "throughout the hall.",
    "carpet" :
    "The plush carpet is soft underfoot, muffling your steps as you walk.",
  ]));
}
