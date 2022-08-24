/* FTPD.c

Tacitus @ LPUniversity
22-JUNE-06
File Transfer Protocal Server

*/

#include <socket.h>

inherit DAEMON ;

#define LOG_FTPD "ftpd"

void write_log(string msg);
void setup();
void login(int fd);
void disconnect_user(int fd);
void parse_cmd(int fd, string msg);
void verifyAuth(int fd);
void command_close_callback(int fd);
varargs int prepare_data_con(int fd, int type, string fullPath, string fileName, int opt);
void send_data(int fd);
void rec_data(int fd, mixed msg);
void send_directoryListing(int fd);

mapping clients, dataConnections;
int server_socket, userCount;

void create()
{
    write_log("The FTPD has been created. Setup in 3 seconds.\n");
    call_out("setup", 3);
}

void write_log(string msg)
{
    string formatedTime;

    sscanf(ctime(time()), "%*s %s", formatedTime);
    log_file(LOG_FTPD, "[" + formatedTime + "] " + msg);
    if(find_player("tacitus")) tell_object(find_player("tacitus"), "[%^BOLD%^" + formatedTime + "%^RESET%^] " + msg);
}

void setup()
{
    int socketErrorCode;

    clients = ([]);
    dataConnections = ([]);

    server_socket = socket_create(STREAM, "command_read_callback", "command_close_callback");

    if(server_socket < 0)
    {
        write_log("Fatal Error: Unable to creat socket for control connection.\n\t"
            + socket_error(server_socket) + "\n");
        write_log("FTPD shutting down. Fatal error during setup.\n");
        destruct(this_object());
    }

    socketErrorCode = socket_bind(server_socket, __PORT__ - 1);

    if(socketErrorCode < 0)
    {
        write_log("Fatal Error: Unable to bind to port '" + (__PORT__ - 1) + "'. May be in use.\n");
        write_log("FTPD shutting down. Fatal error during setup.\n");
        socket_close(server_socket);
        destruct(this_object());
    }

    socketErrorCode = socket_listen(server_socket, "command_listen_callback");

    if(socketErrorCode < 0)
    {
        write_log("Fatal Error: Unable to create listener.\n");
        write_log("FTPD shutting down. Fatal error during setup.\n");
        socket_close(server_socket);
        destruct(this_object());
    }

    write_log("FTPD setup is now complete on fd '" + server_socket + "'.\n\t Accepting connections.\n");
}

void command_listen_callback(int fd)
{
    int new_fd;

    new_fd = socket_accept(fd, "command_read_callback", "command_close_callback");

    if(new_fd < 0)
    {
        write_log("FTPD failed to accept new connection\n\t"
            + socket_error(new_fd));
        return;
    }

    userCount++;
    clients += ([ new_fd : (["last_data" : time(), "mode" : "STREAM", "auth" : 0 ]) ]);

    write_log("Accepted new connection to command connection.\n\t" +
        "New connection assigned fd '" + new_fd + "'.\n");
    login(new_fd);
}

void login(int fd)
{
    string motd;

    motd = read_file("/adm/etc/ftpd_motd");
    if(stringp(motd))
        socket_write(fd, sprintf("220- %s", replace_string(motd, "\n", "\n220- "))[0..<6]);
    socket_write(fd, sprintf(
        "220 %s Mud FTP Server (LPUniFTPD) ready.\n", capitalize(mud_name())));

    clients[fd]["address"] = socket_address(fd)[0..(strsrch(socket_address(fd), " "))];

    if(!clients[fd]["address"])
    {
        socket_write(fd, "451 Fatal Error. Unable to get address.\n"
                    "221 Terminating connection due to server error.\n");
        disconnect_user(fd);
    }
}

void disconnect_user(int fd)
{
    socket_close(fd);
    command_close_callback(fd);
}

void command_read_callback(int fd, string msg)
{
    string *commands;

    if(!clients[fd]) return 0;

    if(!msg) msg = "";
    msg = replace_string(msg, "\r", "");
    commands = explode(msg, "\n") - ({ 0 });

    foreach(string cmd in commands) parse_cmd(fd, cmd);
}

void command_close_callback(int fd)
{
    if(!clients[fd]) return;

    write_log((clients[fd]["username"] ? "User '" + capitalize(clients[fd]["username"])
     : "Fd '" + fd) + "' has disconnected.\n");

    if(clients[fd]["data_fd"] && dataConnections[clients[fd]["data_fd"]])
    {
        socket_close(clients[fd]["data_fd"]);
        map_delete(dataConnections, clients[fd]["data_fd"]);
    }

    map_delete(clients, fd);
}


void parse_cmd(int fd, string msg)
{
    string cmd, *args;

    args = explode(msg, " ");
    cmd = lower_case(args[0]);
    args = args[1..];

    write_log("Parsing command: " + cmd + "\n");

    switch(cmd)
    {
        case "user" :
        {
            if(sizeof(args) < 1) return;
            if(clients[fd]["auth"])
            {
                clients[fd]["auth"] = 0;
                map_delete(clients[fd], "username");
            }

            if(!devp(args[0]))
            {
                socket_write(fd, sprintf("530 User %s access denied.\n", args[0]));
                disconnect_user(fd);
                return;
            }

            socket_write(fd, sprintf("331 Password required for %s.\n", args[0]));
            clients[fd]["username"] = args[0];
            return;
        }

        case "pass" :
        {
            if(clients[fd]["auth"] || !clients[fd]["username"])
            {
                socket_write(fd, "503 Login with USER first.\n");
                return;
            }

            if(FINGER_D->get_user(clients[fd]["username"])->query_password()
                != crypt(implode(args, " "), implode(args, " ") ))
            {
                socket_write(fd, "530 Login incorrect.\n");
                disconnect_user(fd);
                return;
            }

            else
            {
                clients[fd]["auth"] = 1;

                write_log("User '" + capitalize(clients[fd]["username"])
                    + "' authenticated from " + clients[fd]["address"]
                     + " on fd '" + fd + "'.\n");

                if(!directory_exists(user_path(clients[fd]["username"])))
                {
                    socket_write(fd, "230 No Directory! Logging in with home=/ftp.\n");
                    clients[fd]["cwd"] = "/ftp";
                }

                else clients[fd]["cwd"] = user_path(clients[fd]["username"]);

                if(file_exists("/adm/etc/ftp_login"))
                {
                    string loginNews = read_file("/adm/etc/ftp_login");
                    if(stringp(loginNews))
                        socket_write(fd, sprintf("230- %s",
                            replace_string(loginNews, "\n", "\n230- "))[0..<6]);
                }

                socket_write(fd, sprintf("230 User %s logged in succesfully.\n",
                    clients[fd]["username"]));

                return;
            }
        }

        case "syst" :
        {
            verifyAuth(fd);
            socket_write(fd, sprintf("215 MUD Type: %s\n", driver_version()));
            break;
        }

        case "port":
        {
            string *command;
            int i, port;

            verifyAuth(fd);

            command = explode( implode( args[0..], " " ), "," );

            if (sizeof( command ) < 6)  socket_write( fd, "550 Failed command.\n" );

            else
            {
                clients[fd]["data_address"] = implode(command[ 0..3 ], ".");
                sscanf(command[4], "%d", i);
                port = i << 8;
                i = to_int(command[5]);
                port += i;
                clients[fd]["data_port"] = port;
                socket_write(fd, "200 PORT command successful.\n");
                write_log(sprintf("User commanded port: %s %d\n",
                    clients[fd]["data_address"], clients[fd]["data_port"]));
                clients[fd]["pasv"] = 0;
                }

                return;
           }

           case "pasv" :
           {
               object i3 = find_object("/adm/daemons/chmodules/chdmod_i3.c");
               string ip;
               mapping mud_data;

               verifyAuth(fd);

               if(!objectp(i3))
               {
                   write_log("I3 object not found. Unable to enter passive mode!\n");
                   socket_write(fd, "550 Failed command.\n");
                   return;
               }

               mud_data = i3->get_mudlist();

               if(!mud_data[mud_name()])
               {
                   write_log("Mud not found in mudlist. Unable to enter passive mode!\n");
                   socket_write(fd, "550 Failed command.\n");
                   return;
               }

               ip = mud_data[mud_name()][1];
              ip = implode(explode(ip, ".") + ({ "23", "114" }), ",");

              write_log("Pasv send on fd '" + fd + "': " + ip + "\n");

              clients[fd]["pasv"] = 1;
               if(prepare_data_con(fd, 0))
                   socket_write(fd, sprintf("227 Entering Passive Mode (%s).", ip));

               return;
           }

         case "retr" :
         {
            string path;

            verifyAuth(fd);

            path = resolve_path(clients[fd]["cwd"], implode(args, " "));

            if(file_size(path) == -2)
            {
                socket_write(fd, sprintf("550 %s: Not regualr file", implode(args, " ")));
                return;
            }

            if(file_size(path) == -1)
            {
                socket_write(fd, sprintf("550 %s: File does not exist", implode(args, " ")));
                return;
            }

            if(!master()->valid_read(path, FINGER_D->get_user(clients[fd]["username"]), "ftp"))
            {
                socket_write(fd, sprintf("550 %s: Permission denied.", implode(args, " ")));
                return;
            }

            write_log(sprintf("User %s requested file %s on mode %s.\n",
                clients[fd]["username"], path, clients[fd]["mode"]));

            prepare_data_con(fd, 1, path, implode(args, " "));
            return;
        }

        default :
        {
            socket_write(fd, sprintf("500 %s: Unknown command", cmd));
            return;
        }
    }
}

void verifyAuth(int fd)
{
    if(!clients[fd]["auth"])
    {
        write_log("Improper use of service by " + clients[fd]["address"] +
            "\n\tNot authenticated when attempting to use protected command.\n");
        disconnect_user(fd);
    }
}

varargs int prepare_data_con(int fd, int type, string fullPath, string fileName, int opt)
{
    int new_fd, err;
    mapping data;

    if(clients[fd]["data_fd"])
    {
        socket_write(fd, "425 Can't open socket connection.\n");
        return 0;
    }

    write_log("Mode for data con from '" + fd + "' set to: " + clients[fd]["mode"] + "\n");
    if(clients[fd]["mode"] == STREAM)
        new_fd = socket_create(STREAM, "rec_data", "data_close_callback");
    else new_fd = socket_create(STREAM_BINARY, "rec_data", "data_close_callback");

    if(new_fd < 0)
    {
        socket_write(fd, "425 Unable to create data socket.\n");
        write_log("Unable to create data socket for user '"
            + capitalize(clients[fd]["username"]) + "'.\n\t"
             + socket_error(new_fd) + "\n");
        return 0;
    }

    if(clients[fd]["pasv"] && !type)
    {
        err = socket_bind(new_fd, 6002);

        if(err < 0)
        {
            write_log("Unable to bind pasive data socket for user '"
            + capitalize(clients[fd]["username"]) + "'.\n\t"
             + socket_error(new_fd) + "\n");

             socket_write(fd, "425 Unable to create passive data socket.\n");
             socket_close(new_fd);
             return 0;
        }

        err = socket_listen(new_fd, "passive_listen_callback");

        if(err < 0)
        {
            write_log("Error creating listener for pasive data socket for user '"
            + capitalize(clients[fd]["username"]) + "'.\n\t"
             + socket_error(new_fd) + "\n");

             socket_write(fd, "425 Error creating passive data socket.\n");
             socket_close(new_fd);
             return 0;
        }

        return 1;
    }

    else
    {
        err = socket_connect(new_fd, sprintf("%s %d", clients[fd]["data_address"],
            clients[fd]["data_port"]), "rec_data", "send_data");

        if(err < 0)
        {
            write_log("Error connecting to user for data connection for user '"
            + capitalize(clients[fd]["username"]) + "'.\n\t"
             + socket_error(new_fd) + "\n");

            socket_write(fd, "425 Can't build data connection.\n");
            socket_close(new_fd);
            return 0;
        }

    }

    switch(type)
        {
            case 1 :
            {
                data = ([
                    "name"      : fileName,
                    "full_path" : fullPath,
                    "mode"      : clients[fd]["mode"],
                    "parent_fd" : fd,
                    "data"      : read_file(fullPath),
                    "length"    : strlen(read_file(fullPath)),
                    "position"  : 0,
                    "pasv"      : clients[fd]["pasv"],
                ]);

                dataConnections += ([new_fd : data]);

                socket_write(fd,
                    sprintf("150 Opening %s mode data connection for %s (%d bytes).\n",
                       (clients[fd]["mode"] == "STREAM" ? "ASCII" : "BINARY"), fullPath,
                        data["length"]) );
                break;
            }

            case 2 :
            {
                data = ([
                    "name"      : fileName,
                    "full_path" : fullPath,
                    "mode"      : clients[fd]["mode"],
                    "parent_fd" : fd,
                    "append"    : opt,
                    "pasv"      : clients[fd]["pasv"],
                ]);

                dataConnections += ([new_fd : data]);
                break;
            }

            case 3 :
            {
                data = ([
                     "full_path" : fullPath,
                     "mode"      : clients[fd]["mode"],
                     "parent_fd" : fd,
                     "pasv"      : clients[fd]["pasv"],
                 ]);

                 dataConnections += ([new_fd : data]);
                 break;
            }
        }

        return 1;
}

void passive_listen_callback(int fd)
{
    int new_fd;

    new_fd = socket_accept(fd, "rec_data", "send_data");

    if(new_fd < 0)
    {
        write_log("FTPD failed to accept new passive connection\n\t"
            + socket_error(new_fd));
        socket_close(fd);
        return;
    }

    write_log("New passive connection on fd '" + fd + "' assigned fd '"
        + new_fd + "'.\n");

    dataConnections[fd]["pasv_fd"] = fd;
    dataConnections += ([ new_fd : dataConnections[fd] ]);
    map_delete(dataConnections, fd);
}

void send_data(int fd)
{
    mapping data = dataConnections[fd];
    int write_length;


    write_log("Writing data on fd " + fd + "\n");

    if(data["mode"] == "STREAM")
    {
        if(data["position"] + 1024 >= data["length"]-1)
            write_length = data["length"];
        else write_length = 1024;

        while(socket_write(fd, data["data"][(data["position"])..(data["position"] + write_length)])
            == EESUCCESS)
        {

            data["position"] += write_length + 1;
            if(data["position"] >= data["length"])
            {
                socket_write(data["parent_fd"], "226 Transfer complete.\n");
                map_delete(dataConnections, fd);
                map_delete(clients[data["parent_fd"]], "data_fd");
                socket_close(fd);
                write_log("Data transfer to user " + clients[data["parent_fd"]]["username"] +
                    " completed for file " + data["name"] + ".\n");
                return;
            }

            if(data["position"] + write_length >= data["length"] -1)
                write_length = data["length"] - data["position"];
        }

        dataConnections[fd] = data;
    }

    else
    {

    }
}

void rec_data(int fd, mixed msg)
{
    write_log(sprintf("Info: %O\n", msg));
}

void send_directoryListing(int fd)
{

}
