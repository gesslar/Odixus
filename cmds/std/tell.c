/* tell.c

   Gwegster @ LPUniversity
   30-JUNE-05
   a tell command

   Last edited on October 6th, 2006 by Tricky

*/

void send_tell(object o_user, mixed d_user, string d_mud, string msg);
void send_emoteto(object o_user, mixed d_user, string d_mud, string msg);

int main(object caller, object room, string str)
{
    string who, message, where;
    object user, c;

    if(!sscanf(str, "%s@%s: %s", who, where, message))
    {

        if(sscanf(str, ". %s", message))
        {
            who = this_player()->query("retell");
            sscanf(who, "%s@%s", who, where);
        }
        else
        if(!sscanf(str, "%s %s", who, message))
            return(notify_fail("Usage: tell <player> <message>\n"));

    }

    if(!message)
        return(notify_fail("Incorrect syntax, please check the help.\n"));

    if(where)
    {
        c = load_object("/adm/daemons/chmodules/chdmod_i3.c");

        if(message[0] == ':')
        {
            c->send_emoteto(this_player(), who, where, message[1..<1]);
            write(CYN + "You emote to " + capitalize(who) + "@" + where + ": " 
                + NOR + capitalize(this_player()->query_name()) 
                + " " + message[1..<1] + "\n");
        }
        else
        {
            c->send_tell(this_player(), who, where, message);
            write(CYN + "You tell " + capitalize(who) + "@" + where 
                + ": " + NOR + message + "\n");
        }

        this_player()->set("retell", who + "@" + where);

        return 1;
    }

    user = find_player(lower_case(who));
    
    if(!objectp(user)) return notify_fail("Error [tell]: User " + who 
        + " is not found.\n");

    if(user->query_env("away"))
    {
        write("That user is currently away" + (user->query_env("away") != "" ?
            ": (" + user->query_env("away") + ")\n" : ".\n"));

        return 1;
    }

    if(user == this_player())
    {

        if(message[0] == ':')
            write(CYN + "You emote to yourself: " + NOR + user->query_cap_name() 
                + " " + message[1..<1] + "\n" + NOR);
        else
            write(CYN + "You tell yourself: " + NOR + message + "\n" + NOR);

        tell_room(environment(this_player()), 
            capitalize(this_player()->query_name()) 
            + " starts talking to themselves.\n" + NOR, this_player());
        this_player()->set("retell", query_privs(user));

        return 1;
    }

    if(message[0] == ':')
    {
        tell_object(user, CYN + this_player()->query_cap_name() + " " + NOR 
            + message[1..<1] + "\n" + NOR);
        write(CYN + "You emote to " + capitalize(who) + ": " + NOR 
            + this_player()->query_cap_name() + " " + message[1..<1] 
            + "\n" + NOR);
    }
    
    else
    {
        tell_object(user, CYN + capitalize((string)this_player()->query_name()) 
            + " tells you: " + NOR + message + "\n" + NOR);
        write(CYN + "You tell " + capitalize(who) + ": " + NOR + message 
            + "\n" + NOR);
    }

    user->set("reply", query_privs(this_player()));
    this_player()->set("retell", query_privs(user));

    return 1;
}

string help(object caller)
{
    return(HIW + " SYNTAX:" + NOR + " tell <player>[@<mud>: | . | :]<message>\n\n"
      "This command will send a message to the specified player if they\n"
      "are online. For example, if you type 'tell tacitus hey' then\n"
      "he'll see '" + CYN + capitalize(this_player()->query_name()) +
      " tells you:" + NOR + " hey'. If you use 'tell . <message>\n" +
      "it will send the message to the last person that you used tell to talk to.\n" +
      "You may also talk to a user on another mud in the I3 network using the\n" +
      "'tell <user>@<mud>: <message>' syntax. You may also emote to another playter\n" +
      "using the tell <user> :<message> syntax.\n\n" +
      HIW + "See also: " + NOR + "say, channel\n");
}


