 /* chdmod_I3.c

 Tacitus @ LPUniversity
 24-MAR-06
 I3 Channel Module

 Last updated: July 23rd, 2006 by Tricky @ LPUniversity
 Last updated: August 17th-18th, 2006 by Tricky @ LPUniversity
 
 */

 /* NOTICE: THIS IS BETA SOFTWARE - USE AT OWN RISK! <-- WARNING */

 /* Socket stuff */
#include <socket.h>

 /* LOG_I3 */
#include <logs.h>

 /* Channel and finger daemon */
#include <mudlib.h>

 /* For staticf and staticv */
#include <global.h>

 /* I3 defines */
#include <intermud3.h>

 /* Private Variables */
staticv mapping  chanIndex;
staticv mapping  services;
staticv mapping  history, fileHistory;
staticv string  *allow_list;
staticv string  *channelColours = ({
  "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "YELLOW", "WHITE",
});

 /* Socket */
staticv mixed   *msg_buffer = ({ });
staticv int      s, isBlocked = 0;

 /* Heartbeat timer */
staticv int      hb_timer = 0;

 /* Public Variables */
mapping          data;

#ifdef CHECK_NEW_LPUNI_MUD
string          *newLPUniMuds = ({ });
mapping          checkNewLPUniMuds = 0;
#endif

 /* Private prototypes */
staticf void send_channel_listen(string channel, int flag);

 /* Public prototypes */
void create();
void remove();
void initData();
void addRouter(mixed *router);
string channelColour(string chan);
void setHistory(string key, string *val, int append);
#ifdef CHECK_NEW_LPUNI_MUD
void remNewLPUniMuds(string *muds);
#endif
int loadAllowList();
void setup_services();
void setup_other();
void connect();
void reconnect();
void shutDown(int restart_delay);
int setup_socket();
varargs void send_packet(mixed *packet, int quiet);
varargs void send_packets(mixed *packets, int quiet);
int rec_msg(string chan, string usr, string msg);

void send_chan_who_req(mixed m_user, string mudname, string chan);

 /* Functions */
void create()
{
    I3LOG("Notice [I3]: Reloading with previous_object being: " + file_name(previous_object()) + "\n");

    /* Initialize the data mapping and add the router we will use */
    initData();
    addRouter( ({ "*yatmim", "149.152.218.102 23"}) );

    if(file_exists(I3_DATA_FILE + ".o")) restore_object(I3_DATA_FILE);

    if(file_exists(CHANINDEX_FILE + ".o")) chanIndex = restore_variable(read_file(CHANINDEX_FILE + ".o"));
    else chanIndex = 0;

    loadAllowList();

#ifdef CHECK_NEW_LPUNI_MUD
    if(!sizeof(newLPUniMuds)) newLPUniMuds = ({ });

    if(checkNewLPUniMuds)
    {

        foreach(string mudname, mixed val in checkNewLPUniMuds)
            remNewLPUniMuds( ({ mudname }) );
    
        checkNewLPUniMuds = 0;
    }
#endif

    services = ([ ]);
    history = ([ ]);
    fileHistory = ([ ]);

    s = -1;
    isBlocked = 0;
    msg_buffer = 0;

    data["mudlist"] = ([ ]);
    data["listening"] = ({ });
    data["session_keys_by_session"] = ([ ]);
    data["session_keys_by_name"] = ([ ]);

    if(setup_socket() != EESUCCESS)
    {
        I3LOG("Error [I3]: Unable to setup sockets.\n");
        destruct(this_object());

        return;
    }

    setup_services();
    setup_other();
    connect();

#ifdef I3RAW
    CHAN_D->registerModule("I3DEBUG", file_name(this_object()));
    CHAN_D->registerCh("I3DEBUG", "i3raw");

    if(file_exists(HISTORY_FILE + ".i3raw.o"))
        setHistory("i3raw", restore_variable(read_file(HISTORY_FILE + ".i3raw.o")));
    else
        setHistory("i3raw", ({ }) );

#endif

    hb_timer = 0;
    set_heart_beat(1);

    write("Success [I3]: Intermud3 Communications Active (" + query_privs(this_object()) + ").\n");

    save_object(I3_DATA_FILE);
}

void remove()
{
    I3LOG("Notice [I3]: Network Module Destructing.\n");

    shutDown(0);

    return;
}

void heart_beat()
{
    int changed = 0;

    /* Heart beats are 2 seconds long on LPUni by standard */
    hb_timer += 2;

    /* Reset the heartbeat timer every 24 hours */
    if(hb_timer >= 24 * 60 * 60) hb_timer = 0;

#ifdef CHECK_NEW_LPUNI_MUD
    if(hb_timer % 16 == 0)
    {

        if(checkNewLPUniMuds)
        {
    
            foreach(string mud, mixed val in checkNewLPUniMuds)
            {
                string *users = val[0];

                if(!sizeof(users))
                {

                    if(val[1] == 5)
                    {
                        checkNewLPUniMuds[mud][1] += 120;

                        continue;
                    }
                    else
                    if(val[1] > 10)
                    {
                        checkNewLPUniMuds[mud][1]--;

                        continue;
                    }
                    else
                    if(val[1] > 5 && val[1] <= 10) checkNewLPUniMuds[mud][1] = 0;

                    I3LOG("Notice [heart_beat]: Checking for users on the new mud, '" + mud + "', tuned into lpuni.\n");
                    send_chan_who_req("admin", mud, "lpuni");
                    checkNewLPUniMuds[mud][1]++;

                    continue;
                }

                I3LOG("Notice [heart_beat]: Someone is logged into the new mud, '" + mud + "', most likely the new admin, let's welcome them.\n");

                foreach(string user in users)
                    rec_msg("lpuni", "admin", "Welcomes '%^BOLD%^" + capitalize(user) + "@" + mud + "%^RESET%^' to the LPUni fold.");

                if(sizeof(keys(checkNewLPUniMuds)) == 1) checkNewLPUniMuds = 0;
                else map_delete(checkNewLPUniMuds, mud);

                changed = 1;
            }

        }

    }
#endif

    /* Every minute, do some cleanup */
    if(hb_timer % 60 == 0)
    {

        if(data["session_keys_by_name"] != 0 && sizeof(data["session_keys_by_name"]))
        {

            foreach(mixed key, mixed val in data["session_keys_by_name"])
            {
                int diff = time() - val["registered"];
    
                /* Remove the authorization session keys after 10 minutes */
                if(diff > 10 * 60)
                {
                    I3LOG("Notice [heart_beat]: Un-registering session key for " + key + "\n");
                    map_delete(data["session_keys_by_session"], val["session_key"]);
                    map_delete(data["session_keys_by_name"], key);
    
                    changed = 1;
                }
    
            }

        }

    }

    /* If we have altered that data mapping, save it */
    if(changed) save_object(I3_DATA_FILE);
}

void initData()
{
    data = ([
      "router_password": 0,
      "mudlist_id": 0,
      "chanlist_id": 0,
      "mudlist": ([ ]),
      "chanlist": ([ ]),
      "services": ([ ]),
      "other": ([ ]),
    ]);
}

void addRouter(mixed *router)
{

    if(!data["router_list"]) data["router_list"] = ({ });

    data["router_list"] += ({ router, });
}

string channelColour(string chan)
{
   int col = 0;

   for(int i = 0; i < strlen(chan); i++) col += (chan[i] + ((i / 2) + 1));

   col %= sizeof(channelColours);

   return channelColours[col];
}

void setHistory(string key, string *val, int append)
{

    if(member_array(key, keys(history)) == -1) history[key] = ({ });
    if(member_array(key, keys(fileHistory)) == -1) fileHistory[key] = ({ });

    if(append)
    {
        history[key] += val;
        fileHistory[key] += val;
    }
    else
    {
        history[key] = val;
        fileHistory[key] = val;
    }

    if(sizeof(history[key]) > HISTORY_SIZE)
        history[key] = history[key][sizeof(history[key]) - HISTORY_SIZE..<1];

    if(sizeof(fileHistory[key]) > HISTORY_SIZE)
        fileHistory[key] = fileHistory[key][HISTORY_SIZE..<1];

    write_file(CHANINDEX_FILE + ".o", save_variable(chanIndex), 1);
}

void addHistory(string key, string *val)
{
    string *msg = ({ }), saveVar = "";

    if(member_array(key, keys(history)) == -1) history[key] = ({ });
    if(member_array(key, keys(fileHistory)) == -1) fileHistory[key] = ({ });

    foreach(string line in val) msg += ({ sprintf("%s %s", ctime(time()), line) });

    history[key] += msg;
    fileHistory[key] += msg;

    if(sizeof(history[key]) > HISTORY_SIZE)
        history[key] = history[key][sizeof(history[key]) - HISTORY_SIZE..<1];

    if(sizeof(fileHistory[key]) > HISTORY_SIZE)
    {

        foreach(string line in fileHistory[key][0..HISTORY_SIZE - 1])
            saveVar += line;

        write_file(HISTORY_DIR + "/" + key + "-" + sprintf("%08i", chanIndex[key]), saveVar, 1);

        chanIndex[key]++;

        saveVar = "";
        fileHistory[key] = fileHistory[key][HISTORY_SIZE..<1];

        foreach(string line in fileHistory[key])
            saveVar += line;

    }
    else
        foreach(string line in msg) saveVar += line;

    write_file(HISTORY_DIR + "/" + key + "-" + sprintf("%08i", chanIndex[key]), saveVar);
    write_file(CHANINDEX_FILE + ".o", save_variable(chanIndex), 1);
}

void remHistory(string key) { map_delete(history, key); }

void remFileHistory(string key) { map_delete(fileHistory, key); }

mapping getHistory() { return copy(history); }

mapping getFileHistory() { return copy(fileHistory); }

#ifdef CHECK_NEW_LPUNI_MUD
void remNewLPUniMuds(string *muds) { newLPUniMuds -= muds; save_object(I3_DATA_FILE); }
#endif

mapping getMudList() { return copy(data["mudlist"]); }

 /* For compatibility reasons */
mapping get_mudlist() { return getMudList(); }

string *getMuds() { return keys(data["mudlist"]); }

string *getLCMuds()
{
    string *orig_arr = getMuds(), *new_arr = ({ });

    foreach(string name in orig_arr) new_arr += ({ lower_case(name) });

    return new_arr;
}

mapping getChannelList() { return copy(data["chanlist"]); }

string *getChannels() { return keys(data["chanlist"]); }

int loadAllowList()
{
    allow_list = explode(read_file("/adm/etc/i3_channels"), "\n");

    if(!sizeof(allow_list)) return 0;

    return 1;
}

string get_rank(object ob)
{

    if(adminp(ob)) return "%^BOLD%^%^RED%^Admin%^RESET%^";
    else
    if(devp(ob)) return "%^YELLOW%^Developer%^RESET%^";
    else
    return "%^GREEN%^User%^RESET%^";

}

void setup_services()
{
    /* SERVICES SETUP HERE */
    /* Ensure they get added to (mapping) data["services"] */
    data["services"] = ([
      "tell": 1,
      "emoteto": 1,
      "who": 1,
      "finger": 1,
      "locate": 1,
      "channel": 1,
      "auth": 1,
      "ucache": 1,
      "beep": 1, /* http://www.darkwoodinc.com/~tim/i3todo.html */
    ]);
}

void setup_other()
{
    /* OTHER SETUP HERE */
    /* Ensure they get added to (mapping) data["other"] */
    data["other"] = ([
      "uptime": mud_name() + " has been running since " + ctime(time() - uptime()),
    ]);
}

void connect()
{

    if(s == -1) return;

    I3LOG("Notice [I3]: Sending connection details.\n");

    send_packet( ({
      "startup-req-3",
      5,
      mud_name(),
      0,
      data["router_list"][0][0],
      0,
      data["router_password"],
      data["mudlist_id"],
      data["chanlist_id"],

      /* These correspond to the values in a mudlist info_mapping */
      port(),
      0,
      0,
      lib_name() + " " + lib_version(),
      baselib_name() + " " + baselib_version(),
      driver_version(),
      "LPMud",
      open_status(),
      admin_email(),
      data["services"],
      data["other"]
    }) );
}

void reconnect()
{
    initData();
    addRouter( ({ "*yatmim", "149.152.218.102 23"}) );

    if(file_exists(I3_DATA_FILE + ".o")) restore_object(I3_DATA_FILE);

#ifdef CHECK_NEW_LPUNI_MUD
    if(checkNewLPUniMuds)
    {

        foreach(string mudname, mixed val in checkNewLPUniMuds)
            remNewLPUniMuds( ({ mudname }) );
    
        checkNewLPUniMuds = 0;
    }
#endif

    isBlocked = 0;
    msg_buffer = 0;

    if(setup_socket() != EESUCCESS)
    {
        I3LOG("Error [I3]: Unable to setup sockets.\n");
        destruct(this_object());

        return;
    }

    setup_services();
    connect();

    save_object(I3_DATA_FILE);
}

void shutDown(int restart_delay)
{
    save_object(I3_DATA_FILE);

    /* Tell the router we are shutting down */
    send_packet( ({
      "shutdown",
      5,
      mud_name(),
      0,
      data["router_list"][0][0],
      0,
      restart_delay
    }) );

    /* Close the socket. */
    socket_close(s);
    s = -1;

    msg_buffer = 0;
    remove_call_out(0);

    CHAN_D->unregisterModule("I3");
#ifdef I3RAW
    CHAN_D->unregisterModule("I3DEBUG");
#endif

    I3LOG("Success [I3]: I3 connection closed.\n");
}

int setup_socket()
{
    int error;

    error = socket_create(MUD, "read_callback", "close_callback");

    if(error < 0)
    {
        I3LOG("Error [I3/socket]: " + socket_error(s) + "\n");
        socket_close(s);
        s = -1;

        return error;
    }

    s = error;
    I3LOG("Success [I3/socket]: Created (" + s + ").\n");

    I3LOG("Notice [I3/socket]: Connecting to " + data["router_list"][0][0] + ", " + data["router_list"][0][1] + "\n");
    error = socket_connect(s, data["router_list"][0][1], "read_callback", "write_callback");

    if(error != EESUCCESS)
    {
        I3LOG("Error [I3/socket]: " + socket_error(error) + "\n");
        socket_close(s);
        s = -1;

        return error;
    }

    I3LOG("Success [I3]: Connected.\n");

    return EESUCCESS;
}

void close_callback(int fd)
{
    I3LOG("Warning [I3/socket]: Close callback called - Connection Terminated.\n");
    I3LOG("Notice [I3/socket]: Re-establishing connection in 30 seconds.\n");

    save_object(I3_DATA_FILE);

    socket_close(fd);
    s = -1;

    CHAN_D->unregisterModule("I3");
#ifdef I3RAW
    CHAN_D->unregisterModule("I3DEBUG");
#endif

    msg_buffer = 0;
    remove_call_out(0);
    call_out( (: reconnect :), 30);

    I3LOG("Success [I3]: I3 connection closed.\n");
}

void write_callback(int fd)
{
    int error = EESUCCESS;

    isBlocked = 0;

    while(msg_buffer && error == EESUCCESS)
    {

        switch(error = socket_write(fd, msg_buffer[0]))
        {

            case EESUCCESS: break;

            case EEWOULDBLOCK:
            {
                isBlocked = 1;
                call_out( (: write_callback, fd :), 1);

                return;
            }

            case EECALLBACK:
            {
                isBlocked = 1;

                break;
            }

            case EEALREADY:
            {
                isBlocked = 1;

                return;
            }

            default:
            {
                I3LOG("Error [I3/write_callback] on send: " + socket_error(error) + "\n");

                save_object(I3_DATA_FILE);
                msg_buffer = 0;
                remove_call_out(0);

                return;
            }

        }

        if(sizeof(msg_buffer) == 1) msg_buffer = 0;
        else msg_buffer = msg_buffer[1..];

    }

}

varargs void send_packet(mixed *packet, int quiet)
{
    int error;

    if(s == -1) return;

    if(!quiet) I3LOG("Notice [I3]: Sending '" + packet[SERVICE] + "'\n");

    error = socket_write(s, packet);

    if(isBlocked)
    {

        if(msg_buffer) msg_buffer += ({ packet });
        else msg_buffer = ({ packet });

        return;
    }

    switch(error)
    {

        case EESUCCESS: return;

        case EEALREADY:
        {
            isBlocked = 1;

            if(msg_buffer) msg_buffer += ({ packet });
            else msg_buffer = ({ packet });

            return;
        }

        case EEWOULDBLOCK:
        {
            isBlocked = 1;

            if(msg_buffer) msg_buffer += ({ packet });
            else msg_buffer = ({ packet });

            call_out( (: write_callback, s :), 1);

            return;
        }

        case EECALLBACK:
        {
            isBlocked = 1;

            return;
        }

        default:
        {
            I3LOG("Error [I3/send_packet] on send: " + socket_error(error) + "\n");

            save_object(I3_DATA_FILE);
            msg_buffer = 0;
            remove_call_out(0);

            return;
        }

    }

}

varargs void send_packets(mixed *packets, int quiet)
{
    foreach(mixed *packet in packets) send_packet(packet, quiet);
}

void read_callback(int fd, mixed packet)
{
    string func, err;
#ifdef I3RAW
    string i3rawMsg;
#endif

    if(!sizeof(packet)) return;

#ifdef I3RAW
    i3rawMsg = sprintf("%O", packet);
    i3rawMsg = replace_string(i3rawMsg, "\n", "");
    i3rawMsg = replace_string(i3rawMsg, "\t", " ");
    i3rawMsg = replace_string(i3rawMsg, "      ", " ");
    i3rawMsg = replace_string(i3rawMsg, "    ", " ");
    i3rawMsg = replace_string(i3rawMsg, "  ", " ");
    i3rawMsg = sprintf("[%s] %s\n", "%^BOLD%^%^RED%^i3raw%^RESET%^", i3rawMsg);
    CHAN_D->rec_msg("i3raw", sprintf("%s", i3rawMsg));
    addHistory("i3raw", ({ i3rawMsg }) );
#endif

    if(!arrayp(packet)) err = "packet not array";
    else
    if(sizeof(packet) < D_USER) err = "packet size too small";
    else
    if(stringp(packet[D_MUD]) && packet[D_MUD] != mud_name())
        err = "wrong destination mud";
    else
    if(!stringp(packet[O_MUD])) err = "originating mud not a string";
    else
    if(!stringp(packet[SERVICE])) err = "SERVICE is not a string";

    if(err)
    {
        I3LOG("Error [I3]: " + err + ".\n" + sprintf("%O", packet) + "\n");
        return;
    }

    I3LOG("Notice [I3]: Rec '" + packet[SERVICE] + "' from '" + packet[O_MUD] + "'.\n");

    func = "rec_" + packet[0];
    func = replace_string(func, "-", "_");

    if(function_exists(func, this_object()))
        call_other(this_object(), func, packet);
    else
        call_other(this_object(), "rec_unsupported", packet);

}

int isAllowed(string channel, string usr, int flag)
{

    if(flag)
    {

        if(member_array(channel, data["listening"]) == -1)
        {

#ifdef I3RAW
            if(channel != "i3raw")
            {
#endif
                send_channel_listen(channel, 1);
#ifdef I3RAW
            }
            else
            {
                if(!adminp(usr)) return 0;
            }
#endif

            data["listening"] += ({ channel });
        }

    }
    else
    {

        if(!sizeof(CHAN_D->getTuned(channel)))
        {

#ifdef I3RAW
            if(channel != "i3raw")
#endif
                send_channel_listen(channel, 0);

            data["listening"] -= ({ channel });
        }

    }

    return 1;
}

int rec_msg(string chan, string usr, string msg)
{
    mixed *packet;
    string chan_msg;
    int lines = 15;

    if(msg[0..5] == "/last ")
    {
        sscanf(msg, "/last %d", lines);
        msg = "/last";

        if(lines > HISTORY_SIZE) lines = HISTORY_SIZE;

    }

    switch(msg) /* We could do some neat stuff here! */
    {

        case "/last":
        {
            object ob = find_player(usr);

            if(!sizeof(history[chan]))
                tell_object(ob, "I3: Channel " + chan + " has no history yet.\n");
            else
                foreach(string histLine in history[chan][(sizeof(history[chan]) - lines)..<1])
                    tell_object(ob, histLine);

            return 1;
        }
        case "/all":
        {
            object ob = find_player(usr);

            if(!sizeof(history[chan]))
                tell_object(ob, "I3: Channel " + chan + " has no history yet.\n");
            else
                foreach(string histLine in history[chan][(sizeof(history[chan]) - HISTORY_SIZE)..<1])
                    tell_object(ob, histLine);

            return 1;
        }

    }

#ifdef I3RAW
    if(chan == "i3raw")
        return 0;
#endif

    if(msg[0..0] == ":")
    {
        packet = ({
          "channel-e",
          5,
          mud_name(),
          lower_case(usr),
          0,
          0,
          chan,
          capitalize(usr),
          "$N " + msg[1..<1]
        });

        msg = " " + msg[1..<1];
    } else {
        packet = ({
          "channel-m",
          5,
          mud_name(),
          lower_case(usr),
          0,
          0,
          chan,
          capitalize(usr),
          msg
        });

        msg = ": " + msg;
    }

    send_packet(packet);

    chan_msg = sprintf("[%s] %s%s\n", "%^BOLD%^%^" + channelColour(chan) + "%^" + chan + "%^RESET%^", capitalize(usr), msg);
    CHAN_D->rec_msg(chan, chan_msg);
    addHistory(chan, ({ chan_msg }) );

    return 1;
}

void rec_startup_reply(mixed *packet)
{
    I3LOG("Notice [I3]: Initialising connection to the I3 router.\n");

    if(sizeof(packet) != 8)
    {
        I3LOG("Error [I3]: We don't like startup-reply packet size. Should be 8 but is " + sizeof(packet) + "\n");
        return;
    }

    if(!sizeof(packet[6]))
    {
        I3LOG("Error [I3]: We don't like the absence of packet element 6.\n");
        return;
    }

    if(packet[6][0][0] == data["router_list"][0][0])
    {
        data["router_list"] = packet[6];
        data["router_password"] = packet[7];
    }
    else
    {
        data["router_list"] = packet[6];

        /* Connect to the new server */
        remove();
        reconnect();
        save_object(I3_DATA_FILE);

        return;
    }

    I3LOG("Success [I3]: Connection established to I3 router.\n");

    CHAN_D->registerModule("I3", file_name(this_object()));

    save_object(I3_DATA_FILE);
}

 /* I3 service handlers */

#include "/adm/daemons/chmodules/i3services/auth.c"
#include "/adm/daemons/chmodules/i3services/beep.c"
#include "/adm/daemons/chmodules/i3services/channel.c"
#include "/adm/daemons/chmodules/i3services/emoteto.c"
#include "/adm/daemons/chmodules/i3services/error.c"
 //#include "/adm/daemons/chmodules/i3services/file.c"
#include "/adm/daemons/chmodules/i3services/finger.c"
#include "/adm/daemons/chmodules/i3services/locate.c"
 //#include "/adm/daemons/chmodules/i3services/mail.c"
#include "/adm/daemons/chmodules/i3services/mudlist.c"
 //#include "/adm/daemons/chmodules/i3services/oob.c"
#include "/adm/daemons/chmodules/i3services/tell.c"
#include "/adm/daemons/chmodules/i3services/ucache.c"
#include "/adm/daemons/chmodules/i3services/who.c"

void rec_unsupported(mixed *packet)
{
    I3LOG("Fatal [I3]: Service handler not available.\n" + sprintf("%O", packet) + "\n");
    return;
}
