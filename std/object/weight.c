/**
 * @file /std/object/weight.c
 * @description Handles mass and weight mechanics for objects in the game.
 *
 * @created 2024-02-18 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-02-18 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

#include <weight.h>

/**
 * The mass of this object in standard units.
 *
 * @type {int}
 */
int _mass;

/**
 * Returns the current mass of the object.
 *
 * @returns {int} The mass in standard units
 */
int query_mass() {
  return _mass;
}

/**
 * Sets the mass of the object to a new absolute value.
 *
 * Updates the environment's mass tracking if applicable.
 * Will not allow setting negative mass values.
 *
 * @param {int} new_mass - The new mass value to set
 * @returns {int} 1 if successful, 0 if failed
 */
int set_mass(int new_mass) {
  int delta;

  if(new_mass < 0)
    return 0;

  // If our new mass is less than 0, we cannot set the mass.
  delta = new_mass - _mass;

  return adjust_mass(delta);
}

/**
 * Adjusts the mass of the object by a relative amount.
 *
 * Updates the environment's mass and capacity tracking when required.
 * Verifies that the environment can accommodate the change.
 * Has no effect if USE_MASS mud configuration is disabled.
 *
 * @param {int} delta - The amount to adjust by (positive or negative)
 * @returns {int} 1 if adjustment succeeded, 0 if failed
 */
int adjust_mass(int delta) {
  object env;

  if(!mud_config("USE_MASS"))
    return 1;

  // We're not changing mass. So, that's all right.
  if(delta == 0)
    return 1;

  // If we have an environment, we need to check if we can adjust its
  // mass and fill and if so, then we can adjust our mass.
  if(env = environment()) {
    if(!env->ignore_mass())
      if(!env->adjust_mass(delta))
        return 0;

    if(!env->ignore_capacity())
      if(!env->adjust_fill(delta))
        if(!env->ignore_mass()) {
          env->adjust_mass(-delta);
        return 0;
        }
  }

  _mass += delta;

  return 1;
}
