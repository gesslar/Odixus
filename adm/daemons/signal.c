/**
 * @file /adm/daemons/signal.c
 *
 * Signal daemon that provides a system-wide event notification system.
 * Manages registration, dispatch, and cleanup of signal handlers (slots).
 *
 * Signal slots are managed pairs of objects and functions that respond
 * to specific signal types. The system ensures:
 * - Only valid objects and functions can register
 * - Automatic cleanup of invalid slots
 * - Safe dispatch of signals to all registered handlers
 * - Persistence across reboots via swap daemon
 *
 * @created 2024-07-21 - Gesslar
 * @last_modified 2024-07-21 - Gesslar
 *
 * @history
 * 2024-07-21 - Gesslar - Created
 */

inherit STD_DAEMON;

// Forward declarations
private nomask void invalidate_slots();
public nomask int register_slot(int sig, object ob, string func);
public nomask int unregister_slot(int sig, object ob);
public nomask void dispatch_signal(int sig, mixed arg...);

private nosave mapping slots = ([]);

/**
 * Initializes the signal daemon.
 *
 * Loads persisted slots from swap daemon, starts periodic cleanup,
 * and performs initial slot validation.
 */
void setup() {
  set_no_clean(1);

  slots = SWAP_D->swap_out("signal");

  set_heart_beat(60);

  invalidate_slots();
}

/**
 * Registers an object's function to receive a specific signal.
 *
 * @param {int} sig - The signal type to register for
 * @param {object} ob - The object to receive the signal
 * @param {string} func - The function to call when signal is received
 * @returns {int} Status code indicating success or failure reason
 */
public nomask int register_slot(int sig, object ob, string func) {
  if(previous_object() != simul_efun())
    return SIG_SLOT_INVALID_CALLER;

  if(nullp(sig))
   return SIG_MISSING_SIGNAL;

  if(nullp(ob))
    return SIG_MISSING_OBJECT;

  if(nullp(func))
    return SIG_MISSING_FUNCTION;

  if(!objectp(ob))
    return SIG_INVALID_OBJECT;

  if(!function_exists(func, ob))
    return SIG_INVALID_FUNCTION;

  if(!mapp(slots[sig]))
    slots[sig] = ([]);

  slots[sig][ob] = func;

  return SIG_SLOT_OK;
}

/**
 * Unregisters an object from receiving a specific signal.
 *
 * @param {int} sig - The signal type to unregister from
 * @param {object} ob - The object to unregister
 * @returns {int} Status code indicating success or failure reason
 */
public nomask int unregister_slot(int sig, object ob) {
  mapping sig_slot;

  if(previous_object() != simul_efun())
    return SIG_SLOT_INVALID_CALLER;

  if(nullp(sig))
    return SIG_MISSING_SIGNAL;

  if(nullp(ob))
    return SIG_MISSING_OBJECT;

  if(!objectp(ob))
    return SIG_INVALID_OBJECT;

  sig_slot = slots[sig];
  if(of(ob, sig_slot))
    map_delete(sig_slot, ob);

  if(!sizeof(sig_slot))
    map_delete(slots, sig);
  else
    slots[sig] = sig_slot;

  return SIG_SLOT_OK;
}

/**
 * Removes invalid slots from the signal system.
 *
 * A slot is considered invalid if:
 * - The object no longer exists
 * - The function no longer exists in the object
 */
private nomask void invalidate_slots() {
  mapping new_slots = ([ ]);

  if(!mapp(slots))
    slots = ([ ]);

  foreach(int sig, mapping slot in slots) {
    foreach(object ob, string func in slot) {
      if(objectp(ob) && function_exists(func, ob)) {
        if(!mapp(new_slots[sig]))
          new_slots[sig] = ([ ]);

        new_slots[sig][ob] = func;
      }
    }
  }

  slots = new_slots;
}

/**
 * Dispatches a signal to all registered handlers.
 *
 * Safely calls each registered function with the provided arguments.
 * Errors in individual handlers are caught and logged without affecting
 * other handlers.
 *
 * @param {int} sig - The signal to dispatch
 * @param {mixed} arg... - Arguments to pass to the signal handlers
 */
public nomask void dispatch_signal(int sig, mixed arg...) {
  mapping sig_slot = slots[sig];

  if(previous_object() != simul_efun())
    return;

  if(!mapp(sig_slot))
    return;

  foreach(object ob, string func in sig_slot) {
    if(objectp(ob) && function_exists(func, ob)) {
      string e;

      catch(call_other(ob, func, arg...));

      if(e)
        log_file("SIGNAL_ERROR", "Error in signal dispatch: " + e);
    }
  }
}

/**
 * Periodic cleanup of invalid slots.
 */
void heart_beat() {
  invalidate_slots();
}

/**
 * Cleanup handler that persists slots before shutdown.
 *
 * Validates slots one final time and saves them to the swap daemon.
 */
void unsetup() {
  invalidate_slots();
  SWAP_D->swap_in("signal", slots);
}
