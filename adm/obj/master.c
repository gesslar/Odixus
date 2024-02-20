/* master.c

 Tacitus @ LPUniversity
 April 15th, 2005
 master object

 Last edited on July 14th, 2006 by Tacitus

*/

/* Preprocessor Statement */

#include <config.h>
#include <localtime.h>

/* inherits */

inherit "/adm/obj/master/valid";

/* Functions */

private nosave mapping errors = ([]);

void create() {
    // In master/valid.c
    parse_group();
    parse_access();
}

void flag(string str) {
    debug_message("Flags disabled.\n");
}

protected object connect(int port) {
    object login_ob;
    mixed err;

    err = catch(login_ob = new(LOGIN_OB));

    if(err) {
        write("I'm sorry, but it appears that mud is not functional at the moment.\n");
        write(err);
        destruct(this_object());
    }
    return login_ob;
}

protected void epilog(int load_empty) {
    string str, *lines, err;
    int i, time;
    string out = "" ;

    set_privs(this_object(), "[master]");
    lines = explode_file("/adm/etc/preload");

    if(!sizeof(lines)) return;

    for (i = 0; i < sizeof(lines); i++) {
        out = "" ;
        out += "Preloading : " + lines[i] + "..." ;
        err = catch(load_object(lines[i]));

        if(err != 0) {
            out += "\nError " + err + " when loading " + lines[i];
        } else {
            time = time() - time;
            out += "Done (" + time/60 + "." + time % 60 + ")";
        }

        debug_message(out);
    }
    call_out_walltime("tune_into_error", 0.01) ;
}

void tune_into_error() {
    CHAN_D->tune("error", query_privs(this_object()), 1);
}

protected void log_error(string file, string message) {
    string username;

    if(this_player()) username = this_player()->name();
    if(stringp(username)) {
        string path = user_path(username);
        if(directory_exists(path)) {
            write_file(path + "log", "\n" + message);
            if(this_player()->query_env("error_output") != "disabled")
                tell_object(this_player(), message);
        }
    }

    log_file("compile", message);
}

// Blatanly stolen from Lima
int different(string fn, string pr) {
    sscanf(fn, "%s#%*d", fn);
    fn += ".c";
    return (fn != pr) && (fn != ("/" + pr));
}

string trace_line(object obj, string prog, string file, int line) {
    string ret;
    string objfn = obj ? file_name(obj) : "<none>";

    ret = objfn;
    if(different(objfn, prog))
          ret += sprintf(" (%s)", prog);
    if(file != prog)
        ret += sprintf(" at %s:%d\n", file, line);
    else
        ret += sprintf(" at line %d\n", line);
    return ret;
}

varargs string standard_trace(mapping mp, int flag) {
    string ret;
    mapping *trace;
    int i, n;

    ret = ctime(time());
    ret += "\n";
    ret += mp["error"] + "Object: " + trace_line(mp["object"], mp["program"], mp["file"], mp["line"]);
    ret += "\n";
    trace = mp["trace"];

    n = sizeof(trace);

    for (i = 0; i < n; i++) {
        if(flag) ret += sprintf("#%d: ", i);
        ret += sprintf("'%s' at %s",
            trace[i]["function"],
            trace_line(trace[i]["object"], trace[i]["program"], trace[i]["file"], trace[i]["line"])
        );
    }
    return ret;
}

string error_handler(mapping mp, int caught) {
    string ret;
    string logfile = caught ? mud_config("LOG_CATCH") : mud_config("LOG_RUNTIME") ;
    string what = mp["error"];
    string userid;

    ret = "---\n" + standard_trace(mp);
    write_file(logfile, ret);

    // If an object didn't load, they get compile errors.  Don't spam
    // or confuse them
    if(what[0..23] == "*Error in loading object")
        return 0;

    if(this_user()) {
        userid = query_privs(this_user());
        if(!userid || userid == "")
            userid = "(none)";
        printf("%sTrace written to %s\n", what, logfile);
        errors[userid] = mp;
    } else {
        userid = "(none)";
    }
    errors["last"] = mp;

    // Strip trailing \n, and indent nicely
    what = replace_string(what[0.. < 2], "\n", "\n         *");

    CHAN_D->send_msg(
        "error",
        query_privs(this_object()),
        sprintf("(%s) Error logged to %s\n%s\n" +
            capitalize(userid),
            logfile,
            what,
            trace_line(mp["object"], mp["program"], mp["file"], mp["line"])
        )
    );
    return 0;
}

protected void crash(string crash_message, object command_giver, object current_object) {
    foreach (object ob in users()) {
        tell_object(ob, "Master object shouts: Damn!\nMaster object tells you: The game is crashing.\n");
        catch(ob->save_user());
    }

    log_file("shutdown", MUD_NAME + " crashed on: " + ctime(time()) +
        ", error: " + crash_message + "\n");

    log_file("crashes", MUD_NAME + " crashed on: " + ctime(time()) +
        ", error: " + crash_message + "\n");

    if(command_giver) {
        log_file("crashes", "this_player: " + file_name(command_giver) + " :: " + command_giver->name() + "\n");
    }

    if(current_object) {
        log_file("crashes", "this_object: " + file_name(current_object) + "\n");
    }

}

string get_save_file_name(string file, object who)
{
    return "/tmp/ed_SAVE_" + who->name() + "#" + file + random(1000);
}

string privs_file(string filename) {
    string temp;
    if(sscanf(filename, "/adm/daemons/%s", temp)) return "[daemon]";
    if(sscanf(filename, "/adm/obj/%s", temp)) return "[adm_obj]";
    if(sscanf(filename, "/adm/%s", temp)) return "[admin]";
    if(sscanf(filename, "/cmds/adm/%s", temp)) return "[cmd_admin]";
    if(sscanf(filename, "/cmds/file/%s", temp)) return "[cmd_file]";
    if(sscanf(filename, "/cmds/object/%s", temp)) return "[cmd_object]";
    if(sscanf(filename, "/cmds/wiz/%s", temp))return "[cmd_wiz]";
    if(sscanf(filename, "/cmds/%s", temp)) return "[cmd]";
    if(sscanf(filename, "/home/%*s/%s/%*s", temp)) return "[home_" + temp + "]";
    if(sscanf(filename, "/open/%s", temp)) return "[open]";
    if(sscanf(filename, "/std/%s", temp)) return "[std_object]";
    if(sscanf(filename, "/obj/mudlib/%s", temp)) return "[mudlib_object]";
    if(sscanf(filename, "/obj/%s", temp)) return "[gen_obj]";
    else return "object";
}

string object_name(object ob) {
    if(ob->name()) return ob->name();
    return file_name(ob);
}

mixed compile_object(string file) {
    return VIRTUAL_D->compile_object(file);
}

string make_path_absolute(string file) {
    file = resolve_path((string)this_player()->query_cwd(), file);
    return file;
}

void log_file(string file, string msg) {
    int size;

    if(query_privs(previous_object()) == "[open]") return;

    size = file_size(log_dir() + file);
    if(size == -2) return;
    if(size > 50000) {
        mixed *localtime_now = localtime(time());
        string t1;
        string backup;
        int ret = sscanf(file, "%s.log", t1);

        if(ret == 0)
            backup  =
                sprintf("archive/%s-%02d%02d%02d%02d",
                    file,
                    localtime_now[LT_MON],
                    localtime_now[LT_MDAY],
                    localtime_now[LT_HOUR],
                    localtime_now[LT_MIN],
                );
        else
            backup  =
                sprintf("archive/%s-%02d%02d%02d%02d.log",
                    t1,
                    localtime_now[LT_MON],
                    localtime_now[LT_MDAY],
                    localtime_now[LT_HOUR],
                    localtime_now[LT_MIN],
                );
            rename(log_dir() + file, log_dir() + backup);
    }

    write_file(log_dir() + file, msg);
}

int save_ed_setup(object user, mixed config) {
    user->set("ed_setup", config);
    return 1;
}

int retrieve_ed_setup(object user) {
    return user->query("ed_setup");
}

mapping get_mud_stats() {
    return MSSP_D->get_mud_stats();
}
