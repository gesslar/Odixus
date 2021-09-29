//shutdown_d.c

//Tactius @ LPUniversity
//22-OCT-05
//Shutdown daemon

//Nothing fancy, but it works :) -- Tacitus

//Last edited on October 23rd, 2005 by Tacitus

#include <logs.h>

int start(int minutes, int type);
int stop();
void check();
string getStatus();

int startTime;
int endTime;
int type;
int true = 0;

int start(int minutes, int typeIn)
{
     int i;
     object *users = users();
     
     type = typeIn;
     if(previous_object() != find_object("/cmds/adm/shutdown.c") && previous_object() != find_object("/cmds/adm/reboot.c")) return 0;
     if(true) return 0;
  
     message("warning",
          sprintf("%sThe mud will be %s %s.\n%s",
          HIY, type ? RED + "shutting down" + NOR + HIY : NOR + MAG + "rebooting" + NOR + HIY,
          minutes ?  "in " CYN + minutes + NOR + HIY +" minutes" : HIW + "now" + NOR,
          NOR),     
          users()
     );

     if(minutes == 0)
     {
          log_file(LOG_SHUTDOWN, sprintf(ctime(time()) + ": " + mud_name() + "%s", type ? " shutting down.\n" : " rebooting.\n"));
          log_file(LOG_LOGIN, sprintf("Driver %s on " + ctime(time()) + ". All users forced to quit.\n", type ? "shut down" : "rebooted"));
          for(i = 0; i < sizeof(users); i ++) users[i]->save_user();
                message("warning", HIW + "Warning: The mud is going down now!\n" + NOR, users());
          type ? shutdown(-1) : shutdown(0);
     }

     else
     {
          startTime = time();
          endTime = time() + (60 * minutes);
          true = 1;
          call_out("check", 1);
     }
     return 1;
}

int stop()
{
     if(!true) return 0;
     if(previous_object() != find_object("/cmds/adm/shutdown.c") && previous_object() != find_object("/cmds/adm/reboot.c")) return 0;
     message("warning", HIW + "Shutdown/Reboot Canceled.\n" + NOR, users());
     remove_call_out("check");
     true = 0;
     return 1;

}

void check()
{
     int timeLeft;
     int i;
     object *users = users();
     if(!true) return;
     timeLeft = endTime - time();
     if(timeLeft <= 0)
     {
          message("warning", HIW + "Warning: The mud is going down now!\n" + NOR, users());
          log_file(LOG_SHUTDOWN, sprintf(ctime(time()) + ": " + mud_name() + "%s", type ? " shutting down.\n" : " rebooting.\n"));
          for(i = 0; i < sizeof(users); i ++) users[i]->save_user();
          log_file(LOG_LOGIN, sprintf("Driver %s on " + ctime(time()) + ". All users forced to quit.\n", type ? "shut down" : "rebooted"));
          type ? shutdown(-1) : shutdown(0);
     }
     
     if((timeLeft % 300) == 0 && timeLeft < 1200 || timeLeft == 30 || timeLeft == 10)
     {
          message("warning",
          sprintf("%sThe mud will be %s %s %s.\n%s",
          HIY, type ? RED + "shutting down" + NOR + HIY : NOR + MAG + "rebooting" + NOR + HIY,
          "in " CYN + (timeLeft / 60)  + NOR + HIY +" minutes and", CYN + (timeLeft % 60) + NOR + HIY + " seconds",
          NOR),
          users()
          );

     }
     call_out("check", 1);

}

string getStatus()
{     
     if(true) return sprintf("The mud will be %s in %d minutes and %d seconds.\n", type ? "shutting down" : "rebooting", (endTime - time()) / 60, (endTime - time()) % 60  );
}
