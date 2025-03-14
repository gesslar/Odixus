//shutdown.c

//Tacitus @ LPUniversity
//01-JULY-05
//Admin command

//Recoded on October 22nd, 2005 by Tacitus
//Last edited on October 22nd, 2005 by Tacitus

#include <logs.h>
#include <shutdown.h>

inherit STD_CMD;

mixed main(object caller, string arg) {
     string status;
     int time;
     status = shutdown_d()->get_status();

     if(!adminp(previous_object()))
          return _error("Access denied.");

     if(!arg) {
          if(status)
               _info("Shutdown: " + status);
          else
               _info("There is no shutdown or reboot currently in progress.");
          return 1;
     }

     if(arg == "stop") {
          if(!status)
               return _error("There is no shutdown or reboot currently in progress.");
          else shutdown_d()->stop();
               log_file(LOG_SHUTDOWN, capitalize(caller->query_real_name()) + " canceled the sequence (" + time + "m) on " + ctime(time()) + "\n");
          return 1;
     } else {
          if(arg == "now")
               time = 0;
          else
               time = to_int(arg);

          if(time == 0 && arg != "now" && arg != "0")
               return _info("SYNTAX: shutdown [<stop>||<time>/now]");

          log_file(LOG_SHUTDOWN, capitalize(caller->query_real_name()) + " started shutdown sequence (" + time + "m) on " + ctime(time()) + "\n");

          shutdown_d()->start(time, SYS_SHUTDOWN);
          return 1;
     }
}

string help(object caller) {
    return (" SYNTAX: shutdown [<stop>||<time>/now]\n\n" +
    "This command allows you to start a shutdown of the mud. You\n"
    "can either supply the ammount of time intill you the shutdown\n"
    "is to occur or stop the current shutdown/reboot by giving the\n"
    "argument stop. You may also give the argument now to shutdown\n"
    "the mud as soon as the command is given.\n\n"
    "It is important to remember that this command will shutdown the\n"
    "the mud and will not come back up. If you wish for the mud to\n"
    "restart afterwards, then look at the reboot command.\n\n" +
    "See also: reboot\n");
}
