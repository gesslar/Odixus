/* socket_ob.c

 Tacitus @ LPUniversity
 06-MAY-06
 Standard socket object

*/

/* Last updated June 19th, 2006 by Tacitus */

/* This object represents a MudOS UDP or TCP socket.
 * You can clone this object and it'll do all the socket
 * stuff for you.
 */

#include <socket.h>

nosave int type;
nosave int s = -1;
nosave int is_blocked = 0;
nosave mixed *queue = ({});
nosave address;

nosave function read_callback, close_callback;

void remove();

varargs int create_socket(int socket_type, mixed p1, function read_func, function close_func)
{
     int err;

     if(functionp(read_func)) read_callback = read_func;
     if(functionp(close_func)) close_callback = close_func;

     address = p1;

     switch(socket_type)
     {
          case SOCKET_STREAM :
          {
               s = socket_create(STREAM, "read_callback", "close_callback");

               if(s < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               err = socket_connect(s, p1, "read_callback", "write_callback");

               if(err < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               break;
          }

          case SOCKET_STREAM_LISTEN :
          {
               s = socket_create(STREAM, "read_callback", "close_callback" );

               if(s < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               err = socket_bind(s, p1);

               if(err < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               err = socket_listen(s, "listen_callback");

               if(err < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               break;

          }

          case SOCKET_DATAGRAM :
          {
               s = socket_create(DATAGRAM, "udp_read_callback");

               if(s < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               err = socket_bind(s, p1);

               if(err < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               break;
          }

          case SOCKET_MUD :
          {
               s = socket_create(MUD, "read_callback", "close_callback" );

               if(s < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               err = socket_connect(s, p1, "read_callback", "write_callback");

               if(err < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               break;

          }

          case SOCKET_MUD_LISTEN :
          {
               s = socket_create(MUD, "read_callback", "close_callback" );

               if(s < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               err = socket_bind(s, p1);

               if(err < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               err = socket_listen(s, "listen_callback");

               if(err < 0)
               {
                    socket_close(s);
                    destruct(this_object());
                    return error("Error [socket_ob]: " + socket_error(s) + "\n");
               }

               break;

          }

          case SOCKET_ACQUIRE :
          {
               break;
          }

          default :
          {
               error("Error [socket_ob]: Unknown socket type " + socket_type + ".\n");
          }
     }
}

private nomask void listen_callback(int fd)
{
     object new_socket;
     int err;

     fd = socket_accept(fd, "read_callback", "write_callback");
     new_socket = clone_object(base_name(this_object()) + ".c", SOCKET_ACQUIRE,  read_callback,  close_callback);
     err = socket_release(fd, new_socket, "release_callback");
     if(err < 0)  error("Error [socket_ob]: " + socket_error(err) + "\n");
}

private nomask void release_callback(int fd)
{
     int err;

     s = fd;
     err = socket_acquire(s, "read_callback", "write_callback", "close_callback");
     if(err < 0) error("Error [socket_ob]: " + socket_error(err) + "\n");

     catch(evaluate(read_callback, this_object(), 0));
}

private nomask void read_callback(int fd, mixed message)
{
     catch(evaluate(read_callback, this_object(), message));
}

private nomask void udp_read_callback(int fd, mixed message, string address)
{
     catch(evaluate(read_callback, this_object(), message, address));
}

private nomask void close_callback(int fd)
{
     if(functionp(close_callback)) catch(evaluate(close_callback, this_object()));
     remove();
}

private nomask void write_callback(int fd)
{
     int error;
     is_blocked = 0;

     while(sizeof(queue) > 0)
     {
          error = socket_write(s, queue[0]);

          switch(error)
          {
               case EESUCCESS :
               {
                    queue = queue[1..];
                    continue;
               }

               case EEWOULDBLOCK :
               {
                    is_blocked = 1;
                    call_out((: write_callback, s :), 1);
                    return;
               }

               case EECALLBACK :
               {
                    is_blocked = 1;
                    return;
               }

               default :
               {
                    return;
               }
          }
     }
}

void send_packet(mixed *packet)
{
     int error;

     error = socket_write(s, packet);

     if(is_blocked)
     {
          queue += ({ packet });
          return;
     }

     switch(error)
     {
          case EESUCCESS :
          {
               return;
          }

          case EEALREADY :
          {
               is_blocked = 1;
               queue += ({ packet });
               return;
          }

          case EEWOULDBLOCK :
          {
               is_blocked = 1;
               queue += ({ packet });
               call_out((: write_callback, s :), 1);
               return;
          }

          case EECALLBACK :
          {
               is_blocked = 1;
               return;
          }

          default :
          {
               return;
          }
     }
}

nomask mixed *get_address()
{
    string tmp;
    string host;
    int port;

    tmp = socket_address(s);
    sscanf(tmp, "%s %d", host, port);
    return ({ host, port });
}

int status()
{
     switch (type)
     {
          case SOCKET_STREAM:
          printf("%O: listening at %O\n", this_object(), address);
          printf("    read_func=%O  close_func=%O\n", read_callback, close_callback);
          break;

          case SOCKET_STREAM_LISTEN:
          printf("%O: connected to %O\n", this_object(), address);
          printf("    read_func=%O  close_func=%O\n", read_callback, close_callback);
          break;

          case SOCKET_DATAGRAM:
          printf("%O: UDP at %O\n", this_object(), address);
          printf("    read_func=%O\n", read_callback);
          break;

          case SOCKET_MUD_LISTEN:
          printf("%O: (mud) listening at %O\n", this_object(), address);
          printf("    read_func=%O  close_func=%O\n", read_callback, close_callback);
          break;

          case SOCKET_MUD:
          printf("%O: (mud) connected to %O\n", this_object(), address);
          printf("    read_func=%O  close_func=%O\n", read_callback, close_callback);
          break;

          case SOCKET_ACQUIRE:
          printf("%O: Accepted connection from %s\n", this_object(),
          socket_address(s));
          printf("    read_func=%O  close_func=%O\n", read_callback, close_callback);
          break;
     }

     if(sizeof(queue)) printf("Queue: %O\n", queue);

     return 1;
}

void remove()
{
     if( s >= 0 ) socket_close(s);
}
