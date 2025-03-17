/**
 * @file /std/object/init.c
 * @description Manages object initialization callbacks when objects encounter
 *              each other in the game world.
 *
 * @created 2024-02-29 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-02-29 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

inherit __DIR__ "command";

private nosave mixed *_inits = ({});

/**
 * Driver apply function called when a moving object enters the same
 * environment as this object.
 *
 * Triggers an "init" event and passes the entering object.
 */
void init() {
  event(({this_object()}), "init", previous_object());
}

/**
 * Event handler for initialization events.
 *
 * Executes all registered init callbacks when objects encounter each other.
 *
 * @param {object} prev - The previous environment
 * @param {object} ob - The object that triggered the init event
 */
void event_init(object prev, object ob) {
  int sz;

  sz = sizeof(_inits);
  for(int i = 0; i < sz; i++) {
    if(valid_function(_inits[i]))
      catch((*_inits[i])(ob));
    else
      catch(call_other(this_object(), _inits[i], ob));
  }
}

/**
 * Registers a callback to be executed during object initialization.
 *
 * @param {function|string} fun - Function pointer or method name to call
 */
void add_init(mixed fun) {
  if(valid_function(fun) || stringp(fun))
    _inits += ({ fun });
}

/**
 * Removes a previously registered initialization callback.
 *
 * @param {function|string} fun - Function pointer or method name to remove
 */
void remove_init(mixed fun) {
  if(valid_function(fun) || stringp(fun))
    _inits -= ({ fun });
}

/**
 * Removes all initialization callbacks.
 */
void clear_inits() {
  _inits = ({});
}

/**
 * Returns a copy of all registered initialization callbacks.
 *
 * @returns {mixed*} Array of initialization callbacks
 */
mapping query_inits() {
  return copy(_inits);
}
