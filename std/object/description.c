/**
 * @file /std/object/description.c
 * @description Functions and variables for object descriptions
 *
 * @created 2024-01-31 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-01-31 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

#include <description.h>

// Variables
private mixed short, long;
private nosave mapping extra_short = ([ ]), extra_long = ([ ]);

/**
 * Sets the short description of an object.
 *
 * The short description appears in room inventories and brief views.
 * Can be a string or function that returns a string.
 *
 * @param {string|function} str - The short description or function to call
 * @returns {int} Always returns 1
 */
int set_short(mixed str) {
  short = str;

  if(environment())
    event(environment(), "gmcp_item_update", environment());
}

/**
 * Returns the short description of this object.
 *
 * If short is a function, it will be called with the viewer as an argument.
 *
 * @param {object} [viewer=this_body()] - The object viewing this description
 * @returns {string} The short description, or 0 if none exists
 */
string query_short(object viewer: (: this_body() :)) {
  mixed result;

  result = short;

  if(valid_function(result))
    result = (*result)(viewer);

  if(!stringp(result))
    result = 0;

  return result;
}

/**
 * Sets the long description of an object.
 *
 * The long description appears when examining the object.
 * Can be a string or function that returns a string.
 *
 * @param {string|function} str - The long description or function to call
 * @returns {int} Always returns 1
 */
int set_long(mixed str) {
  long = str;
}

/**
 * Returns the long description of this object.
 *
 * If long is a function, it will be called with the viewer as an argument.
 *
 * @param {object} [viewer=this_body()] - The object viewing this description
 * @returns {string} The long description, or 0 if none exists
 */
string query_long(object viewer: (: this_body() :)) {
  mixed result;

  result = long;

  if(valid_function(result))
    result = (*result)(viewer);

  if(!stringp(result))
    result = 0;

  return result;
}

/**
 * Adds an extra short description with an identifier.
 *
 * Extra short descriptions appear in addition to the main short description.
 *
 * @param {string} id - Unique identifier for this extra description
 * @param {string|function} str - The extra description or function to call
 */
void add_extra_short(string id, mixed str) {
  if(!mapp(extra_short))
    extra_short = ([ ]);

  extra_short[id] = str;
}

/**
 * Returns all extra short descriptions.
 *
 * @returns {string*} Array of all extra short descriptions
 */
string *query_extra_shorts() {
  string *cles;
  string *result;

  if(!mapp(extra_short))
    extra_short = ([ ]);

  cles = keys(extra_short);
  cles = sort_array(cles, 1);

  result = map(cles, (: query_extra_short :));

  return result;
}

/**
 * Removes an extra short description.
 *
 * @param {string} id - Identifier of the extra description to remove
 */
void remove_extra_short(string id) {
  if(!mapp(extra_short))
    extra_short = ([ ]);

  map_delete(extra_short, id);
}

/**
 * Returns a specific extra short description.
 *
 * If the extra short is a function, it will be called without arguments.
 *
 * @param {string} id - Identifier of the extra description
 * @returns {string} The extra short description, or 0 if none exists
 */
string query_extra_short(string id) {
  string result;

  if(!mapp(extra_short))
    extra_short = ([ ]);

  if(valid_function(extra_short[id]))
    result = (*extra_short[id])();

  if(!stringp(result))
    result = 0;

  return result;
}

/**
 * Adds an extra long description with an identifier.
 *
 * Extra long descriptions appear in addition to the main long description.
 *
 * @param {string} id - Unique identifier for this extra description
 * @param {string|function} str - The extra description or function to call
 */
void add_extra_long(string id, mixed str) {
  if(!mapp(extra_long))
    extra_long = ([ ]);

  extra_long[id] = str;
}

/**
 * Removes an extra long description.
 *
 * @param {string} id - Identifier of the extra description to remove
 */
void remove_extra_long(string id) {
  if(!mapp(extra_long))
    extra_long = ([ ]);

  map_delete(extra_long, id);
}

/**
 * Returns a specific extra long description.
 *
 * If the extra long is a function, it will be called without arguments.
 *
 * @param {string} id - Identifier of the extra description
 * @returns {string} The extra long description, or 0 if none exists
 */
string query_extra_long(string id) {
  string result;

  if(!mapp(extra_long))
    extra_long = ([ ]);

  if(valid_function(extra_long[id]))
    result = (*extra_long[id])();

  if(!stringp(result))
    result = 0;

  return result;
}

/**
 * Returns all extra long descriptions.
 *
 * @returns {string*} Array of all extra long descriptions
 */
string *query_extra_longs() {
  string *cles;
  string *result;

  if(!mapp(extra_long))
    extra_long = ([ ]);

  cles = keys(extra_long);
  cles = sort_array(cles, 1);

  result = map(cles, (: query_extra_long :));

  return result;
}
