/* channel.c

 Tacitus @ LPUniversity
 02-OCT-05
 Standard Command

 - Rewrote 15-JAN-06 by Tacitus @ LPUni
*/

/* Last edited on July 14th, 2006 by Tacitus */

int main(object caller, object room, string args)
{
     string cmd, arg, argChannel;
     string *arr, multipleTune;
     int i;

     if(!args) return(notify_fail("Sytnax: channel <list/show/tune> <argument>\n"));

     sscanf(args, "%s", cmd);
     sscanf(args, "%s %s", cmd, arg);
     sscanf(args, "%s %s %s", cmd, arg, argChannel);

     switch(cmd)
     {
          case "list" :
          {
               if(!arg)
               {
                   arr = CHAN_D->getMods();

                   if(sizeof(arr) == 1)
                       write("%^BOLD%^The following channel module is installed:%^YELLOW%^\n\t"
                           + arr[0] + "%^RESET%^\n");

                else write("%^BOLD%^The following channel modules are installed: %^YELLOW%^\n\t"
                    + implode(arr[0..(sizeof(arr) - 2)], ", ") + ", " + arr[sizeof(arr) - 1]
                    + "%^RESET%^\n");

                write("\n%^MAGENTA%^You may list the channels registered by each module by typing:%^RESET%^\n");
                write("\tchannel list [<network_name> || all]\n");
                   return 1;
               }

               arr = CHAN_D->getChls(arg);
               if(sizeof(arr) > 0)
               {
                    if(sizeof(arr) == 1) write("The channels are as follows for '"
                        + arg + "':\n\t" + arr[0] + "\n");
                    else if(sizeof(arr) > 7) printf("The channels are as follows for '"
                        + arg + "':\n\t%s,\n\t%s, and %s\n", implode(arr[0..6], ", "),
                         implode(arr[7..(sizeof(arr)-2)], ", "), arr[sizeof(arr)-1]);
                    else printf("The channels are as follows for '" + arg
                        + "':\n\t%s, and %s.\n", implode(arr[0..sizeof(arr)-2], ", "),
                         arr[sizeof(arr)-1]);
                    return 1;
               }
               write("Channel: No channels found for group " + arg + "\n");
               return 1;
          }
          case "show" :
          {
               if(!arg) return(notify_fail("Sytnax: channel <list/show/tune> <argument>\n"));
               if(!CHAN_D->valid_ch(arg)) return notify_fail("Channel: Channel " + arg + " does not exist.\n");
               arr = CHAN_D->getTuned(arg);
               if(sizeof(arr) > 0)
               {
                    if(sizeof(arr) == 1) write("The the following user is tuned into '" + arg + "':\n\t" + capitalize(arr[0]) + "\n");
                    else printf("The following users are tuned into '" + arg + "':\n\t%s, and %s.\n", implode(arr[0..sizeof(arr)-2], ", "), arr[sizeof(arr)-1]);
                    return 1;
               }
               write("Channel: No users tuned into channel " + arg + "\n");
               return 1;
          }
          case "tune" :
          {
               if(!argChannel) return(notify_fail("Syntax: channel tune <in/out> <channel/network>\n"));

               if(arg == "in")
               {
                    multipleTune = CHAN_D->getChls(argChannel);
                    if(sizeof(multipleTune) > 0)
                    {
                         for(i = 0; i < sizeof(multipleTune); i ++)
                         {
                              if(CHAN_D->tune(multipleTune[i], this_player()->query_name(), 1)) write("Tune: Tuned into channel " + multipleTune[i] + "\n");
                              else write("Tune: Channel " + multipleTune[i] + " does not exist.\n");
                         }
                         return 1;
                    }
                    if(CHAN_D->tune(argChannel, this_player()->query_name(), 1)) write("Tune: Tuned into channel " + argChannel + "\n");
                    else write("Tune: Channel " + argChannel + " does not exist.\n");
                    return 1;
               }

               if(arg == "out")
               {
                    multipleTune = CHAN_D->getChls(argChannel);
                    if(sizeof(multipleTune) > 0)
                    {
                         for(i = 0; i < sizeof(multipleTune); i ++)
                         {
                              if(CHAN_D->tune(multipleTune[i], this_player()->query_name(), 0)) write("Tune: Tuned out of channel " +  multipleTune[i] + "\n");
                              else write("Tune: Channel " + multipleTune[i] + " does not exist.\n");
                         }
                         return 1;
                    }
                    if(CHAN_D->tune(argChannel, this_player()->query_name(), 0)) write("Tune: Tuned out of channel " + argChannel + "\n");
                    else write("Tune: Channel " + argChannel + " does not exist.\n");
                    return 1;
               }
          }

          default :
          {
               return(notify_fail("Sytnax: channel <list/show/tune> <argument>\n"));
          }
     }
}

string help(object caller)
{
     string *modList = CHAN_D->getMods(), mods;


     if(!sizeof(modList)) mods = "There is no networks currently installed.";
     if(sizeof(modList) == 1) mods = modList[0];
     else mods = implode(modList[0..(sizeof(modList)-2)], ", ") + ", " + modList[sizeof(modList) - 1];
     return(" %^BOLD%^SYNTAX:%^RESET%^ channel <list/show/tune> <argument>\n\n"
     "This command allows you to interact with the different channel networks\n"
     "available here on " + capitalize(mud_name()) + ". More specifically,\n"
     "it currently allows you to list the different channels that you can\n"
     "tune into, display who is tuned into a channel, and the ability to\n"
     "tune in and out of channels/networks. If you are wondering what a\n"
     "network is, it is a group of channels. You may notice that different\n"
     "networks have different channels, different features, and different\n"
     "users/muds communicating through it. Currently, the following networks\n"
     "are installed on " + capitalize(mud_name()) + ":\n" +
     "\t" + mods + "\n");
}
