/**
 * @file /std/object/heartbeat.c
 * @description Provides event management that synchronizes with an object's
 *              heartbeat cycle. Events can be attached to execute during
 *              heartbeats, which may vary in frequency due to buffs or status
 *              effects.
 *
 * @created 2024-03-03 - Gesslar
 * @last_modified 2025-03-16 - GitHub Copilot
 *
 * @history
 * 2024-03-03 - Gesslar - Created
 * 2025-03-16 - GitHub Copilot - Added documentation
 */

private nomask nosave mixed *hb_events = ({});

public varargs void add_hb(mixed f, int interval);
void remove_hb(mixed f);

/**
 * Adds an event to execute during heartbeat cycles.
 *
 * @param {function|string} f - Function pointer or method name to call
 * @param {int} [interval=1] - Number of heartbeats between executions
 */
public varargs void add_hb(mixed f, int interval) {
  if(!valid_function(f) && !stringp(f))
    return;

  remove_hb(f);

  if(nullp(interval))
    interval = 1;

  hb_events += ({ ({ 0, interval, f }) });
}

/**
 * Removes a previously registered heartbeat event.
 *
 * @param {function|string} f - Function pointer or method name to remove
 */
void remove_hb(mixed f) {
  if(!valid_function(f) && !stringp(f))
    return;

  hb_events = filter(hb_events, (: $1[1] != $2 :), f);
}

/**
 * Evaluates all registered heartbeat events.
 *
 * Called during an object's heartbeat, this function processes all
 * registered events, executing those whose intervals have been reached.
 * Skips execution for user players. Resets counter after execution.
 *
 * @returns {void}
 */
protected evaluate_heart_beat() {
  int i, sz;

  if(userp())
    return;

  for(i = 0, sz = sizeof(hb_events); i < sz; i++) {
    hb_events[i][0]++;
    if(hb_events[i][0] >= hb_events[i][1]) {
      if(stringp(hb_events[i][2])) {
        catch(call_other(this_object(), hb_events[i][2]));
      } else {
        function f;

        if(!valid_function(hb_events[i][2]))
          continue;

        catch {
          f = bind(hb_events[i][2], this_object());
          (*f)();
        };
      }

      hb_events[i][0] = 0;
    }
  }
}

/**
 * Returns a copy of all registered heartbeat events.
 *
 * @returns {mixed*} Array of heartbeat event data
 */
mixed *query_hb() {
  return copy(hb_events);
}
