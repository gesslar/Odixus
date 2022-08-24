/* mob.c

 Tacitus @ LPUniversity
 11-JUNE-06
 Character Object

*/

/* This object represents a charachter in the mud.
 Another object that inherits this represents NPCs
 */

/* Last edited by Tacitus on October 4th, 2006 */

#include <config.h>
#include <driver/origin.h>
#include <logs.h>

inherit OBJECT;
inherit "/std/user/user/alias.c";

/* Global Variables */

string *path;
nosave string *commandHistory = ({});
private object link;

/* Prototypes */

/* Connection functions */

void setup();
void enter_world();
void exit_world();
void net_dead();
void reconnect();

/* User object functions */

varargs int move(mixed ob, int flag);
void restore_user();
void save_user();
void remove();
object query_link();
int set_link(object ob);

/* User environmental variable functions */

int set_env(string var_name, string var_value);
mixed query_env(string var_name);
mapping list_env();

/* User path functions */

string *query_path();
void add_path(string str);
void rem_path(string str);

/* Communication & Interaction functions */

void catch_tell(string message);
void receive_message(string type, string msg);
string process_input(string arg);
int commandHook(string arg);
mixed* query_commands();
int force_me(string cmd);

/* Misc functions */

void write_prompt();

/* Functions */

void create()
{
    if(origin() != ORIGIN_DRIVER && origin() != ORIGIN_LOCAL) return;
    path = ({"/cmds/std/"});
}

/* Connection functions */

void setup()
{
    add_action("commandHook", "", 1);
    set_living_name(query_name());
    set_ids(({query_name()}));
    set_name(query_name());
    set_heart_beat(1);
    enable_commands();
    set("prevent_get", 1);
    if(!query("cwd")) set("cwd", "/doc");
    if(!query_long()) set_long(capitalize(query_name()) + "");
    if(!mapp(query("env_settings"))) set("env_settings", (["colour" : "enabled"]));
    if(!query_env("news_client")) set_env("news_client", "/obj/mudlib/newsclients/std_newsclient.c");
    if(!query_env("auto_tune")) set_env("auto_tune", "all");
    if(!query_env("biff")) set_env("biff", "on");
    if(!query_env("prompt")) set_env("prompt", ">");
}

void enter_world()
{
    string *cmds, *ch;
    int i;
    object news_client, mail_client;
    
    if(!isMember(query_privs(previous_object()), "admin")) return;
    
    ANNOUNCE_CHDMOD->announce_login(query_name());
    
    catch
    {
        news_client = new(query_env("news_client"));
        news_client->move(this_object());
        if(news_client->isNew()) write("\n%^BOLD%^GREEN%^Notice: %^RESET%^There are new news posts.\n\n");
        destruct(news_client);
        mail_client = new(OBJ_MAIL_CLIENT);
        mail_client->move(this_object());
        if(mail_client->has_new_mail()) write("%^BOLD%^%^CYAN%^You have new mail!%^RESET%^\n\n");
        destruct(mail_client);
    };
    
    catch
    {
        ch = explode(query_env("auto_tune"), " ");
        if(sizeof(ch) > 0)
            foreach(string channel in ch)
               force_me("channel tune in " + channel);
    };
    
    
    if(file_size(user_path(query_name()) + ".login") > 0)
    {
        write("\n");
        cmds = explode(read_file(user_path(query_name()) + ".login"), "\n");
        if(sizeof(cmds) <= 0) return;
        for(i = 0; i < sizeof(cmds); i ++) catch(command(cmds[i]));
    }
    
    
    
    set("last_login", time());
    write("\n");
    say(capitalize(query_name()) + " has entered.\n");
}

void exit_world()
{
    string *cmds;
    int i;
    
    if(this_player() != this_object()) return;
    
    if(file_size(user_path(query_name()) + ".quit") > 0)
    {
        cmds = explode(read_file(user_path(query_name()) + ".quit"), "\n");
        if(sizeof(cmds) <= 0) return;
        for(i = 0; i < sizeof(cmds); i ++) catch(command(cmds[i]));
    }
    
    set("last_login", time());
    
    if(environment(this_player())) say((string)capitalize(query_name())
      + " leaves " + mud_name() + ".\n");
    
    ANNOUNCE_CHDMOD->announce_logoff(query_name());
    
    save_user();
}

void net_dead()
{
    if(origin() != ORIGIN_DRIVER) return;
    set("last_login", time());
    save_user();
    if(environment(this_object())) tell_room(environment(this_object()), capitalize(query_name()) + " has gone link-dead.\n");
    set_short(capitalize(query_name()) + " [link dead]");
    log_file(LOG_LOGIN, capitalize(query_name()) + " went link-dead on " + ctime(time()) + "\n");
}


void reconnect()
{
    restore_user();
    set("last_login", time());
    write("Success: Reconnected.\n");
    if(environment(this_object())) tell_room(environment(this_object()), capitalize(query_name()) + " has reconnected.\n", this_player());
    set_short(capitalize(query_name()));
    /* reconnection logged in login object */
}

/* User Object Functions */

void heart_beat()
{
    if(!interactive(this_object()))
    {
        if((time() - query("last_login")) > 3600)
        {
            if(environment(this_object()))
                tell_room(environment(this_object()), capitalize(query_name()) +
                  " fades out of existance.\n");
            log_file(LOG_LOGIN, capitalize(query_name()) + " auto-quit after 1 hour of net-dead at " + ctime(time()) + ".\n");
            destruct(this_object());
        }
    }
    
    else
    {
        /* Prevent link-dead from idle */
        if(query_idle(this_object()) % 60 == 0 && query_idle(this_object()) > 300
                && query_env("keepalive") && query_env("keepalive") != "off")
        {
            send_nullbyte(this_object()) ;
        }
    }
}

void restore_user()
{
    if(!isMember(query_privs(previous_object() ? previous_object() : this_player()), "admin") && this_player() != this_object()) return 0;
    if(isMember(query_privs(previous_object()), "admin") || query_privs(previous_object()) == this_player()->query_name()) restore_object(user_mob_data(query_name()));
}

void save_user()
{
    if(!isMember(query_privs(previous_object() ? previous_object() : this_player()), "admin") && this_player() != this_object()) return 0;
    catch(save_object(user_mob_data(query_name())));
}

varargs int move(mixed ob, int flag)
{
    if(!::move(ob)) return 0;
    set("last_location", base_name(ob));
    if(!flag) command("look");
    return 1;
}

void remove()
{
    if(objectp(query_link())) destruct(query_link());
    ::remove();
}

object query_link()
{
    return link;
}

int set_link(object ob)
{
    if(query_privs(previous_object()) != query_privs(this_object())
        && !adminp(previous_object())) return 0;
    link = ob;
    return 1;
}

/* Environmental Settings */

int set_env(string var_name, string var_value)
{
    mapping data = query("env_settings");
    if(!var_value) map_delete(data, var_name);
    else data += ([var_name : var_value]);
    set("env_settings", data);
    return 1;
}

mixed query_env(string var_name)
{
    mapping data = query("env_settings");
    if(data[var_name]) return data[var_name];
}

mapping list_env()
{
    mapping data = query("env_settings");
    return data;
}


/* User path functions */

string *query_path()
{
    return copy(path);
}

void add_path(string str)
{
    if(!adminp(previous_object()) && this_player() != this_object()) return;
    
    if(member_array(str, path) != -1)
    {
        write("Error [path]: Directory '" + str + "' is already in your path.\n");
        return;
    }
    
    if(str[<1] != '/') str += "/";
    
    if(!directory_exists(str))
    {
        write("Error [path]: Directory '" + str + "' does not exist.\n");
        return;
    }
    
    path += ({str});
}

void rem_path(string str)
{
    if(!adminp(previous_object()) && this_player() != this_object()) return;
    
    if(member_array(str, path) == -1)
    {
        write("Error [path]: Directory '" + str + "' is not in your path.\n");
        return;
    }
    
    path -= ({str});
}

/* Communication & Interaction functions */

void catch_tell(string message)
{
    receive_message("unknown", message);
}

void receive_message(string type, string msg)
{
    if(type != "ignore_ansi")
    {
        if(query_env("colour") == "enabled") msg = find_object(ANSI_PARSER)->parse_pinkfish(msg);
        else msg = ANSI_PARSER->parse_pinkfish(msg, 1);
    }
    
    receive(msg);
}

string process_input(string arg)
{
    return ANSI_PARSER->strip_unsafeAnsi(arg);
}

nomask varargs string *query_commandHistory(int index, int range)
{
        if(this_player() != this_object() && !adminp(previous_object())) return ({});
        if(!index) return commandHistory + ({});
        else if(range) return commandHistory[index..range] + ({});
        else return ({ commandHistory[index] });
}

int commandHook(string arg)
{
    string verb, err, *cmds = ({});
    string custom, tmp;
    object command;
    int i;
    
    if(interactive(this_object())) if(this_player() != this_object()) return 0;

    if(query_env("away"))
    {
        write("You return from away\n");
        set_env("away", 0);
        return 1;
    }
    
    verb = query_verb();
    
    if(sscanf(alias_parse(verb, arg), "%s %s", verb, arg) != 2)
        verb = alias_parse(verb, arg);
    if(arg == "") arg = 0;
    verb = lower_case(verb);
    
    if(arg) commandHistory += ({ verb + " " + arg });
    else commandHistory += ({ verb });

    catch
    {
        if(environment(this_object()))
        {
            if(environment(this_object())->valid_exit(verb))
            {
                if(this_player()->moveAllowed(environment(this_player())->query_exit(verb)))
                {
                    if(this_player()->query_env("move_in") && wizardp(this_player()))
                    {
                        custom = this_player()->query_env("move_in");
                        tmp = custom;
                        tmp = replace_string(tmp, "$N", query_cap_name());
                        tell_room(environment(this_player())->query_exit(verb), capitalize(tmp) + "\n", this_player());
                    }
                    else
                    {
                        tell_room(environment(this_player())->query_exit(verb), capitalize(query_name()) + " has entered the room.\n", this_player());
                    }
            
                    if(this_player()->query_env("move_out") && wizardp(this_player()))
                    {
                        custom = this_player()->query_env("move_out");            
                        tmp = custom;
                        tmp = replace_string(tmp, "$N", query_cap_name());
                        tmp = replace_string(tmp, "$D", verb);
                        tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());
                    }
                    else
                    {
                        tell_room(environment(this_player()), capitalize(query_name())
                            + " leaves through the " + verb + " exit.\n", this_player());
                    }
            
                    write("You move to " + environment(this_player())->query_exit(verb)->query_short() +
                        ".\n\n");
            
                    this_player()->move(environment(this_player())->query_exit(verb));
                    return 1;
                }
        
                else
                {
                    write("Error [move]: Unable to move through that exit.\n");
                    return 1;
                }
            }
        
            if(SOUL_D->request_emote(verb, arg)) return 1;
        }
    
        err = catch(load_object(CHAN_D));
        if(!err)
        {
            if(CHAN_D->snd_msg(verb, query_name(), arg)) return 1;
        }
    };
    
    for(i = 0; i < sizeof(path); i ++)
    {
        if(file_exists(path[i] + verb + ".c"))
            cmds += ({ path[i] + verb });
    }
    
    if(sizeof(cmds) > 0)
    {
        int returnValue;
    
        i = 0;
        while(returnValue <= 0 && i < sizeof(cmds))
        {
            err = catch(command = load_object(cmds[i]));
        
            if(err)
            {
                write("Error: Command " + verb + " non-functional.\n");
                write(err);
                i++;
                continue;
            }
        
            returnValue = command->main(arg);
            i++;
        }
    
        return returnValue;
    }
    
    return 0;
}

mixed* query_commands()
{
    return commands();    
}

int force_me(string cmd)
{
    if(!isMember(query_privs(previous_object()), "admin")) return 0;
    else command(cmd);
}

//Misc functions

void write_prompt()
{
        string prompt = query_env("prompt");

        catch
        {
            if(devp(this_object()))
            { 
                    prompt = replace_string(prompt, "%d",
                        ((query("cwd")[0..(strlen(user_path(query_name())) - 1)] ==
                             user_path(query_name()) ) ? "~/" +
                             query("cwd")[(strlen(user_path(query_name())))..] : query("cwd")));
                    prompt = replace_string(prompt, "%f",
                        (query("cwf")[0..(strlen(user_path(query_name())) - 1)] ==
                             user_path(query_name()) ) ? "~" +
                             query("cwf")[strlen(user_path(query_name()))..] : query("cwf"));
                    prompt = replace_string(prompt, "%u", "" + sizeof(users()));
                    prompt = replace_string(prompt, "%l", file_name(environment(this_player())));
            }

            prompt = replace_string(prompt, "%n", query_cap_name());
            prompt = replace_string(prompt, "%m", mud_name());
            prompt = replace_string(prompt, "%t", ctime(time()));
            prompt = replace_string(prompt, "$n", "\n");
        };

    write(prompt + " ");
}


