/**
 * @file /std/living/act.c
 * @description Call-out based action module
 *
 * @created 2024-08-08 - Gesslar
 * @last_modified 2024-08-08 - Gesslar
 *
 * @history
 * 2024-08-08 - Gesslar - Created
 */

#include <classes.h>

inherit CLASS_ACT ;

private nomask nosave class Act *_acts = ({}) ;

int act(string action, float delay, mixed *cb) {
  int id ;
  string uuid ;
  class Act act ;
  object po ;

  if(!action || !delay || !cb)
    return null ;

  if(intp(delay))
    delay = to_float(delay) ;

  uuid = generate_uuid() ;
  id = call_out_walltime("finish_act", delay, uuid) ;

  if(caller_is(SIMUL_OB))
    po = previous_object(1) ;
  else
    po = previous_object() ;

  act = new(class Act,
    id: id,
    uuid: uuid,
    action: action,
    cb: cb,
    prev: po
  ) ;

  _acts += ({ act }) ;

  return id ;
}

class Act find_act(mixed id) {
  int sz ;

  sz = sizeof(_acts) ;
  if(stringp(id))
    while(sz--) {
      if(_acts[sz].uuid == id)
        return _acts[sz] ;
    } else if(intp(id))
       while(sz--) {
         if(_acts[sz].id == id)
          return _acts[sz] ;
    }

  return null ;
}

varargs class Act pop_act(mixed id) {
  int sz ;

  sz = sizeof(_acts) ;
  if(!sz)
    return null ;

  return pop(ref _acts) ;
}

void finish_act(string action, string uuid) {
  class Act act ;

  if(!classp(act = pop_act(uuid)))
    return ;

  catch(call_back(act.cb, true)) ;
}

void cancel_acts() {
  class Act act ;

  while(classp(act = pop_act())) {
    remove_call_out(act.id) ;
    catch(call_back(act.cb, false)) ;
  }
}

int cancel_act(mixed action) {
  class Act act ;

  if(!classp(act = pop_act(action)))
    return 0 ;

  remove_call_out(act.id) ;
  catch(call_back(act.cb, false)) ;

  return 1 ;
}

mapping query_acts() {
  mapping acts = ([ ]) ;
  int sz ;

  sz = sizeof(_acts) ;
  while(sz--)
    acts[_acts[sz].uuid] = _acts[sz] ;

  return acts ;
}

varargs int is_acting(string action) {
  if(!action)
    return sizeof(_acts) > 0 ;

  return sizeof(filter(_acts, (: $1.action == $(action) :))) > 0 ;
}
