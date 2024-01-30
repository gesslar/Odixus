/* home.c

Tacitus @ LPUniversity
23-OCT-05
Standard Wizard Command

This command will move you to ~/workroom.c if it
exists. You can also provide the name of another
developer to attempt to move into their workroom.

*/

//Last edited on July 22nd, 2006 by Parthenon

inherit CMD ;

int main(object caller, object room, string str)
{
    string custom, tmp;

    if(!str)
    {
    if(file_exists(user_path(this_player()->query_name()) + "workroom.c"))
    {
        if(this_player()->query_env("teleport_out") && wizardp(this_player()))
        {
        custom = this_player()->query_env("teleport_out");

        tmp = custom;
        tmp = replace_string(tmp, "$N", this_player()->query_cap_name());
        tmp = replace_string(tmp, "$D", "home");

        tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());

        }
        else
        tell_room(environment(this_player()), capitalize(this_player()->query_name()) + " teleports home.\n", ({this_player()}));

        this_player()->move(user_path(this_player()->query_name()) + "workroom.c");

        if(this_player()->query_env("teleport_in") && wizardp(this_player()))
        {
        custom = this_player()->query_env("teleport_in");

        tmp = custom;
        tmp = replace_string(tmp, "$N", this_player()->query_cap_name());
        tmp = replace_string(tmp, "$D", "here");

        tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());
        }
        else
        tell_room(environment(this_player()), capitalize(this_player()->query_name()) + " teleports here.\n", ({this_player()}));

        write("You teleported to your workroom.\n");

        return 1;
    }
    else return(notify_fail("Error [home]: You do not have a workroom.\n"));
    }
    else
    {
    str = lower_case(str);
    if(file_exists(user_path(str) + "workroom.c"))
    {
        if(this_player()->query_env("teleport_out"))
        {
        custom = this_player()->query_env("teleport_out");

        tmp = custom;
        tmp = replace_string(tmp, "$N", this_player()->query_cap_name());
        tmp = replace_string(tmp, "$D", capitalize(str) + "'s workroom.");

        tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());

        return 1;
        }
        else
        tell_room(environment(this_player()), capitalize(this_player()->query_name()) + " teleports to someone's workroom.\n", ({this_player()}));

        this_player()->move(user_path(str) + "workroom.c");

        if(this_player()->query_env("teleport_in") && wizardp(this_player()))
        {
        custom = this_player()->query_env("teleport_in");

        tmp = custom;
        tmp = replace_string(tmp, "$N", this_player()->query_cap_name());
        tmp = replace_string(tmp, "$D", "here");

        tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());
        }
        else
        tell_room(environment(this_player()), capitalize(this_player()->query_name()) + "has teleported here.\n");

        write("You teleported to " + capitalize(str) + "'s workroom.\n");

        return 1;
    }

    else return(notify_fail("Error [home]: " + capitalize(str) + " does not have a workroom.\n"));
    }
}

string help(object caller)
{
    return("%^BOLD%^SYNTAX: %^RESET%^workroom [<user>]\n\n"
      "This command will move you to ~/workroom.c if it\n"
      "exists. You can also provide the name of another\n"
      "developer to attempt to move into their workroom.\n");
}
