#include <simul_efun.h>

#include <commands.h>
#include <daemons.h>
#include <type.h>

//overrides.c

//Tacitus @ LPUniversity
//08-APR-05
//Simul-efuns

//Last edited on December 21st, 2005 by Tacitus

/**
 * @efun_override exec
 * @description This function is an override for the efun exec(). It checks
 *              if the object calling the function has admin privs. If it does,
 *              it will execute the function as normal. If it does not, it will
 *              return 0.
 * @param {object} to - The object to exec to.
 * @param {object} from - The object to exec from.
 * @returns {int} - 1 if the exec was successful, 0 if it was not.
 */
int exec(object to, object from) {
    if(is_member(query_privs(previous_object()), "admin") ||
       base_name(previous_object()) == STD_GHOST ||
       userp(previous_object()) ||
       base_name(previous_object()) == CMD_SU ||
       linkp(previous_object()))
        return efun::exec(to, from);
    else
        return 0;
}

/**
 * @efun_override shutdown
 * @description This function is an override for the efun shutdown(). It checks
 *              if the object calling the function has admin privs. If it does,
 *              it will execute the function as normal. If it does not, it will
 *              return 0.
 * @param {int} how - The type of shutdown to perform.
 * @returns {void}
 */
void shutdown(int how) {
    object po = previous_object() ;
    string f = base_name(po) ;

    if(   po != master()
       && po != load_object(SHUTDOWN_D)
    ) return ;

    emit(SIG_SYS_SHUTDOWN) ;
    PERSIST_D->persist_objects() ;
    efun::shutdown(how);
}

/**
 * @simul_efun shutdown_d
 * @description This simul_efun returns the shutdown daemon.
 * @returns {object} - The shutdown daemon.
 */
object shutdown_d() {
    return load_object(SHUTDOWN_D);
}

/**
 * @efun_override set_privs
 * @description This function is an override for the efun destruct(). It checks
 *              if the object calling the function has admin privs. If it does,
 *              it will execute the function as normal. If it does not, it will
 *              return 0.
 * @param {object} ob - The object to destruct.
 * @returns {void}
 */
void set_privs(object ob, string privs) {
    string name;

    if(is_member(query_privs(previous_object()), "admin") || ob = master()) efun::set_privs(ob, privs);
    sscanf(file_name(ob), "/home/%*s/%s/%*s", name);
    if(name == privs) efun::set_privs(ob, privs);
}

/**
 * @efun_override write
 * @description This function is an override of the write() efun. It will
 *              write a message to the player using the message() efun
 *              if this_body(), otherwise it will use the debug_message()
 *              efun.
 * @param {string} msg - The message to write.
 * @returns {void}
 */
void write(string msg) {
    if(this_body()) message("write", msg + "", this_body());
    else debug_message(msg) ;
}

/**
 * @efun_override say
 * @description This function is an override of the say() efun. It will
 *              write a message to the room using the message() efun.
 * @param {string} msg - The message to write.
 * @param {mixed} exclude - The object or array of objects to exclude from
 *                          the message.
 * @returns {void}
 */
varargs void say(string msg, mixed exclude) {
    object me;

    if(this_body()) me = this_body();
    else me = previous_object();

    if(objectp(exclude)) exclude = ({ me, exclude });
    else if(pointerp(exclude)) exclude += ({ me });
    else if(!exclude) exclude = ({ me });
    message("say", msg, environment(me), exclude);
}

/**
 * @efun_override shout
 * @description This function is an override of the shout() efun. It will
 *              write a message to all users using the message() efun.
 * @param {string} msg - The message to write.
 * @param {mixed} exclude - The object or array of objects to exclude from
 *                          the message.
 * @returns {void}
 */
varargs void shout(string msg, mixed exclude) {
    if(objectp(exclude))
        exclude = ({ exclude });
    else if(!pointerp(exclude))
        exclude = ({ });
    if(this_body())
        exclude += ({ this_body() });
    message("shout", msg, users(), exclude);
}

/**
 * @efun_override tell_object
 * @description This function is an override of the tell_object() efun. It will
 *              write a message to the object using the message() efun.
 * @param {object} ob - The object to write the message to.
 * @param {string} msg - The message to write.
 * @param {string} type - The message class of message to write.
 * @returns {void}
 */
varargs void tell_object(mixed ob, mixed msg, mixed type) {
    if(!type) type = "tell_object" ;
    message(type, msg + "", ob);
}

/**
 * @efun_override tell_room
 * @description This function is an override of the tell_room() efun. It will
 *              write a message to the room using the message() efun.
 * @param {mixed} room - The room to write the message to.
 * @param {string} msg - The message to write.
 * @param {mixed} exclude - The object or array of objects to exclude from
 *                          the message.
 * @returns {void}
 */
varargs void tell_room(mixed room, string msg, mixed exclude) {
    if(!exclude || exclude==0) {
        message ("tell_room",msg,room) ;
    } else {
        message("tell_room", msg, room, exclude);
    }
}

/**
 * @efun_override userp
 * @description This function is an override of the userp() efun. Unlike the
 *              efun, this function accepts a null argument and will check
 *              if the previous object is a user if no argument is provided.
 * @param {object} ob - The object to check.
 * @returns {int} - 1 if the object is a user, otherwise 0.
 */
varargs int userp(object ob) {
    if(!ob) ob = previous_object() ;
    return efun::userp(ob);
}

/**
 * @efun_override query_num
 * @description This function is an override of query_num(). It will return
 *              the number in string form.
 * @param {int} x - The number to convert to a string.
 * @param {int} many - If true, the number will be returned in plural form.
 * @returns {string} - The number in string form.
 */
varargs string query_num(int x, int many) {
    string sign;

    if(nullp(x))
        error ("Too few arguments to 'query_num'.") ;

    if(!intp(x))
        error ("Bad argument 1 to 'query_num'.") ;

    sign = x < 0 ? "negative " : "" ;
    x = abs(x) ;

    return sign + efun::query_num(x, many) ;
}

/**
 * @efun_override ctime
 * @description This function is an override of the efun ctime(). It will
 *              return the current time in the format "YYYY-MM-DD HH:MM:SS".
 *              It also will use time() if no argument is provided.
 * @param {int} x - The time to convert to a string.
 * @returns {string} - The time in string form.
 */
varargs string ctime(int x) {
    string fmt ;

    if(nullp(x)) x = time() ;

    fmt = "%Y-%m-%d %H:%M:%S" ;
    return strftime(fmt, x) ;
}

/**
 * @efun_override this_user
 * @description This is an override of the efun this_user(). It will return
 *              the this_body()->query_user().
 * @returns {object} - The this_body()->query_user().
 */
object this_user() {
    _debug("this body = %O\n", this_body()) ;
    return this_body()->query_user();
}

#if 0
private nosave string _empty_buffer = null ;
private mixed _fix_array(mixed *arr) ;
private mapping _fix_mapping(mapping m) ;
private mixed _fix_buffer(mixed args...) ;

private mixed _fix_array(mixed *arr) {
    for (int i = 0; i < sizeof(arr); i++) {
        if (bufferp(arr[i]) && !sizeof(arr[i])) {
            arr[i] = _empty_buffer;
        } else if (arrayp(arr[i])) {
            arr[i] = _fix_array(arr[i]);
        } else if (mapp(arr[i])) {
            arr[i] = _fix_mapping(arr[i]);
        }
    }
    return arr;
}

private mapping _fix_mapping(mapping m) {
    mapping new_map = ([]);
    foreach (mixed key, mixed val in m) {
        if (bufferp(key) && !sizeof(key)) {
            error("Mapping key is an empty buffer");
        }
        if (bufferp(val) && !sizeof(val)) {
            new_map[key] = _empty_buffer;
        } else if (arrayp(val)) {
            new_map[key] = _fix_array(val);
        } else if (mapp(val)) {
            new_map[key] = _fix_mapping(val);
        } else {
            new_map[key] = val;
        }
    }
    return new_map;
}

private nomask mixed _fix_buffer(mixed args...) {
    int sz = sizeof(args);

    for (int i = 0; i < sz; i++) {
        if (bufferp(args[i]) && !sizeof(args[i])) {
            args[i] = _empty_buffer;
        } else if (arrayp(args[i])) {
            args[i] = _fix_array(args[i]);
        } else if (mapp(args[i])) {
            args[i] = _fix_mapping(args[i]);
        }
    }

    return args;
}

string sprintf(string fmt, mixed args...) {
    args = _fix_buffer(args...) ;

    return efun::sprintf(fmt, args...);
}

void printf(string fmt, mixed args...) {
    args = _fix_buffer(args...) ;

    efun::printf(fmt, args...);
}
#endif
