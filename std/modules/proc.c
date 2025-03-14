/**
 * @file /std/modules/proc.c
 * @description Module for handling procs for objects.
 *
 * @created 2024-10-06 - Gesslar
 * @last_modified 2024-10-06 - Gesslar
 *
 * @history
 * 2024-10-06 - Gesslar - Created
 */

#include <proc.h>

private nosave mapping _procs = ([]);
private nosave mapping _cooldowns = ([]);
private nosave float _proc_chance = 15.0;
private nosave int _proc_cooldown = 1;
private nosave int _proc_weight = 100;

/**
 * @module_function set_procs
 * @description Set the procs for an object. You must pass the procs as an
 *              2D array of names and mappings, strings, or function pointers.
 *              * The first element of the array is the name of the proc.
 *              * The second element of the array is the proc as a mapping,
 *                string, or function pointer that follows the same criteria as
 *                the individual elements of the arguments passed to add_proc.
 * @param {mixed[]} procs - The procs to set.
 */
void set_procs(mixed *procs) {
  assert((:pointerp($(procs)):), "Procs must be an array.");
  assert((:sizeof($(procs)) > 0:), "Procs must have at least one element.");

  foreach(mixed proc in procs) {
    assert((:sizeof($(proc)) == 2:), "Proc must have two elements.");
    add_proc(proc[0], proc[1]);
  }
}

/**
 * @module_function add_proc
 * @description Add a proc to an object. You must pass the name of the proc and
 *              the proc as a mapping, a string, or a valid function pointer.
 *              * If you pass a string, it must be a function name in
 *                this_object() that is either public or protected.
 *              * If you pass a function pointer, it must be a valid function.
 *              * If you pass a mapping, it must contain a function property.
 *                The value of this property can be a string, a function
 *                with the same criteria as above (string or function). When
 *                cooldown and weight are not specified, the default values
 *                are used.
 *              * In the case of a string or a function pointer, the cooldown
 *                and weight are set to the default values.
 * @param {string} name - The name of the proc to add.
 * @param {mixed} proc - The proc to add as a mapping, string, or function
 *                       pointer.
 */
void add_proc(string name, mixed proc) {
  assert((:stringp($(name)) || valid_function($(name)):), "Name must be a string or valid function.");
  assert((:nullp(_procs[$(name)]):), "Proc already exists.");
  assert((:mapp($(proc)) || stringp($(proc)) || valid_function($(proc)):), "Proc must be a mapping, string, or valid function.");
  if(mapp(proc))
    assert((:!nullp($(proc["function"])):), "Proc must have a function.");

  if(stringp(proc) || valid_function(proc))
    proc = ([ "function": proc ]);

  if(nullp(proc["cooldown"]))
    proc["cooldown"] = _proc_cooldown;

  if(nullp(proc["weight"]))
    proc["weight"] = _proc_weight;

  _procs[name] = proc;
}

/**
 * @brief Remove a proc from an object.
 * @param {string} name - The name of the proc to remove.
 */
void remove_proc(string name) {
  assert((:stringp($(name)):), "Name must be a string.");

  map_delete(_procs, name);
}

/**
 * @brief Query a proc from an object.
 * @param {string} name - The name of the proc to query.
 * @returns {mapping} The proc.
 */
mapping query_proc(string name) {
  assert((:stringp($(name)):), "Name must be a string.");

  return _procs[name];
}

/**
 * @brief Query all procs from an object.
 * @returns {mapping} The procs.
 */
mapping query_all_procs() {
  return copy(_procs);
}

/**
 * @brief Set the proc chance.
 * @param {float} chance - The chance of a proc.
 */
void set_proc_chance(float chance) {
  assert((:floatp($(chance)):), "Chance must be a float.");
  assert((:$(chance) >= 0.0 && $(chance) <= 100.0:), "Chance must be between 0.0 and 100.0.");

  _proc_chance = chance;
}

/**
 * @brief Query the proc chance.
 * @returns {float} The proc chance.
 */
float query_proc_chance() {
  return _proc_chance;
}

/**
 * @brief Add a proc chance to an object.
 * @param {float} chance - The chance of a proc.
 */
void adjust_proc_chance(float chance) {
  assert((:floatp($(chance)):), "Chance must be a float.");
  assert((:$(chance) >= 0.0 && $(chance) <= 100.0:), "Chance must be between 0.0 and 100.0.");
  assert((:_proc_chance + $(chance) <= 100.0:), "Proc chance must not exceed 100.0.");
  assert((:_proc_chance + $(chance) >= 0.0:), "Proc chance must not be less than 0.0.");

  _proc_chance += chance;
}

/**
 * @brief Check if a proc can occur.
 * @param {object} attacker - The attacker.
 * @param {object} defender - The defender.
 * @returns {string} The name of the proc that can occur, or false if no proc can occur.
 */
string can_proc() {
  mapping procs = ([]);
  int now = time();
  string result;
  float roll;

  _debug("can_proc: Checking for procs.");
  // If there are no procs, return false
  if(!sizeof(_procs))
    return false;
  _debug("can_proc: There are %d procs.", sizeof(_procs));

  // If the proc chance is 0, return false
  if(_proc_chance <= 0.0)
    return false;
  _debug("can_proc: Proc chance is %f.", _proc_chance);

  // Iterate through all procs and check if they can proc, if they have
  // a cooldown
  foreach(string name, mapping proc in _procs) {
    if(proc["cooldown"] > 0)
      if(now - _cooldowns[name] > proc["cooldown"])
        procs[name] = proc["chance"] || 100;
  }
  _debug("can_proc: Procs: %O", procs);

  if(!sizeof(procs))
    return false;
  _debug("can_proc: Final procs: %O", procs);
  // Now let's check which proc can occur. This is based on the weight of
  // the proc.
  result = element_of_weighted(procs);
  _debug("can_proc: Result: %s", result);
  return result;
}

/**
 * @brief Call a proc.
 * @param {string} name - The name of the proc to call.
 * @param {mixed} args - The arguments to pass to the proc function.
 */
varargs void proc(string name, mixed args...) {
  mapping proc = query_proc(name);
  mixed func;

  _debug("proc: Calling proc %s.", name);
  if(nullp(proc)) {
    _debug("proc: Proc %s not found.", name);
    return;
  }

  func = proc["function"];

  if(stringp(func))
    catch(call_other(this_object(), func, args...));
  else if(valid_function(func))
    catch((*func)(args...));
  else
    return;

  _cooldowns[name] = time();
}
