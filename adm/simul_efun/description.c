/**
 * @file /adm/simul_efun/description.c
 *
 * Simul-efun implementation of object description handling functions.
 * Provides standardized methods for retrieving and formatting object
 * descriptions, including both short and long forms with optional extras.
 *
 * @created Unknown
 * @last_modified 2024-03-11
 */

#include <simul_efun.h>

/**
 * Returns an object's short description with optional extras.
 *
 * Retrieves the base short description and appends any extra short descriptions
 * in parentheses. Filters out null or empty extra descriptions.
 *
 * @param {object} ob - Object to get description from, defaults to previous_object()
 * @param {int} [extras=1] - Whether to include extra descriptions (1=yes, 0=no)
 * @returns {string} Formatted short description
 * @example
 * // Basic usage
 * write(get_short(sword));  // "a sharp sword (glowing) (magical)"
 * // Without extras
 * write(get_short(sword, 0));  // "a sharp sword"
 */
varargs string get_short(object ob, int extras) {
  string result;
  string *extra_shorts;

  if(!ob)
    ob = previous_object();

  if(nullp(extras))
    extras = 1;

  result = ob->query_short();
  if(!result)
    return "";

  extras = !!extras;
  if(extras == 1) {
    extra_shorts = ob->query_extra_shorts() || ({ });
    extra_shorts = filter(extra_shorts, (: stringp :));
    extra_shorts = filter(extra_shorts, (: $1 != "" :));
    extra_shorts -= ({ 0 });
    if(sizeof(extra_shorts)) {
      extra_shorts = map(extra_shorts, (: sprintf("(%s)", $1) :));
      result += " " + implode(extra_shorts, " ");
    }
  }

  return result;
}

/**
 * Returns an object's long description with optional extras.
 *
 * Retrieves the base long description and appends any extra long descriptions.
 * Handles formatting of newlines and removes trailing whitespace.
 *
 * @param {object} ob - Object to get description from, defaults to previous_object()
 * @param {int} [extras=1] - Whether to include extra descriptions (1=yes, 0=no)
 * @returns {string} Formatted long description
 * @example
 * // Basic usage
 * write(get_long(room));
 * // Without extras
 * write(get_long(room, 0));
 */
varargs string get_long(object ob, int extras) {
  string result;
  string *extra_longs, *parts;;

  if(!ob)
    ob = previous_object();

  if(nullp(extras))
    extras = 1;

  result = ob->query_long();
  if(!result) return "";

  // remove all double newlines
  parts = explode(result, "\n\n");
  parts = map(parts, function(string part) {
    while(part[<1] == '\n')
      part = part[0..<2];
    return part;
  });

  result = implode(parts, "\n\n");

  extras = !!extras;
  if(extras == 1) {
    extra_longs = ob->query_extra_longs() || ({ });
    extra_longs = filter(extra_longs, (: stringp :));
    extra_longs = filter(extra_longs, (: $1 != "" :));
    extra_longs -= ({ 0 });
    if(sizeof(extra_longs)) {
      extra_longs = map(extra_longs, function(string extra_long) {
        while(extra_long[<1] == '\n')
          extra_long = extra_long[0..<2];

        return extra_long + "\n";
      });

      result += "\n" + implode(extra_longs, "\n");
    }
  }

  while(result[<1] == '\n')
    result = result[0..<2];

  return result;
}
