//goto.c

//Tacitus @ LPUniversity
//29-JUNE-05
//Wizard goto command
//   * Thanks to Icoz

//Last edited on July 22nd, 2006 by Parthenon

inherit STD_CMD ;

int main(object caller, object room, string arg)
{
    string custom, tmp;
    object ob;
    string file;

    if(!arg) return(notify_fail("Error [goto]: Where would you like to go?\n"));

    if(find_player(arg))
    {
        if(environment(find_player(arg)) == environment(this_player())) return(notify_fail("Error [goto]: You are already with '" + capitalize(find_player(arg)->name()) + "'.\n"));

        if(this_player()->query_env("teleport_out") && wizardp(this_player()))
        {
            custom = this_player()->query_env("teleport_out");

            tmp = custom;
            tmp = replace_string(tmp, "$N", this_player()->query_cap_name());
            tmp = replace_string(tmp, "$D", find_player(arg)->query_cap_name());

            tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());
        }
        else
            tell_room(environment(this_player()), capitalize(this_player()->name()) + " teleports to another location.\n", ({ this_player() }) );

        this_player()->move(environment(find_player(arg)));

        if(this_player()->query_env("teleport_in") && wizardp(this_player()))
        {
            custom = this_player()->query_env("teleport_in");

            tmp = custom;
            tmp = replace_string(tmp, "$N", this_player()->query_cap_name());
            tmp = replace_string(tmp, "$D", "here");

            tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());
        }
        else
            tell_room(environment(this_player()), capitalize(this_player()->name()) + " has teleported to this room.\n", this_player());

        write("You have teleported to " + capitalize(find_player(arg)->name()) + " in " + environment(find_player(arg))->query_short() + "\n");

        return 1;
    }
    else
    {
        file = resolve_path(this_player()->query("cwd"), arg);

        if(file[<2..<1] != ".c") file += ".c";
        if(!file_exists(file)) return(notify_fail("Error [goto]: " + file + " does not exist.\n"));
        if(!ob = find_object(file)) ob = load_object(file);
        if(!ob) return(notify_fail("Error [goto]: Unable to move to " + file + ".\n"));
        if(environment(this_player()) == ob) return(notify_fail("Error [goto]: You are already there.\n"));

        if(this_player()->query_env("teleport_out") && wizardp(this_player()))
        {
            custom = this_player()->query_env("teleport_out");

            tmp = custom;
            tmp = replace_string(tmp, "$D", ob->query_short());
            tmp = replace_string(tmp, "$N", this_player()->query_cap_name());

            tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());
        }
        else
            tell_room(environment(this_player()), capitalize(this_player()->name()) + " teleports to another location.\n", ({ this_player() }) );

        this_player()->move(ob);

        if(this_player()->query_env("teleport_in") && wizardp(this_player()))
        {
            custom = this_player()->query_env("teleport_in");

            tmp = custom;
            tmp = replace_string(tmp, "$D", "here");
            tmp = replace_string(tmp, "$N", this_player()->query_cap_name());

            tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player());
        }
        else
            tell_room(environment(this_player()), capitalize(this_player()->name()) + " has teleported to this room.\n", ({ this_player() }) );

        write("You have teleported to " + capitalize(ob->query_short()) + "\n");

        return 1;
    }

    return 0;
}

string help(object caller) {
    return (" SYNTAX: goto <room/user>" + "\n\n" +
      "The goto command allows you to transport to another room by\n"
      "filename or transport to another use. If ther user or room\n"
      "specified is not found, you will not be moved.\n");
}
