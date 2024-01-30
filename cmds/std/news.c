/* news.c

 Tacitus @ LPUniversity
 27-JAN-06
 Std Command

*/

// Last updated on April 15th, 2006 by Tacitus@LPUniversity

inherit CMD ;

int main(object caller, object room, string args)
{
    object client;

    if(adminp(caller) && args == "admin")
    {
        client = clone_object("/adm/obj/news_admin.c");
        client->move(caller);
        client->main_menu();
        return 1;
    }

     client = clone_object("/obj/mudlib/newsclients/std_newsclient.c");
     client->move(caller);

     if(args == "check")
     {
         if(client->isNew())
             write("%^BOLD%^GREEN%^Notice: %^RESET%^There are new news posts.\n");
         else write("No new posts.\n");
         destruct(client);
         return 1;
     }

     client->start_client();
     return 1;
}

string help(object caller)
{
    string help = " %^BOLD%^SYNTAX:%^RESET%^ news [check]\n\n"
    "This command launches your news client which will\n"
    "allow you to read the news available in the different\n"
    "news groups. Insde a group you may type 'h' or 'help'\n"
    "to get specific instructions on how to use the client.\n\n"
    "You can provide the optional argument of 'check' and you'll\n"
    "be notified if there are new posts or not but will not enter\n"
    "the news shell.\n";
    if(adminp(caller)) help +=
    "\nAdmins: If you provide 'admin' as an argument to this\n"
    "command then you will enter the news system admin shell.\n";
    return help;
}
