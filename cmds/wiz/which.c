/* which.c

 Tacitus @ LPUniversity
 05-Oct-06
 Scans command path for command

*/

inherit CMD ;

int main(object caller, object room, string args)
{
    string *commandPath = this_player()->query_path();
    mixed *actions = previous_object()->query_commands();
    mapping aliases = this_player()->get_aliases(1);
    int i, isLocated = 0;

    if(!args) return notify_fail("Error: Syntax: which <verb/command>\n");

    for(i = 0; i < sizeof(commandPath); i ++)
    {
        if(file_exists(commandPath[i] + args + ".c"))
        {
            isLocated = 1;
            write(commandPath[i] + args + "\n");
        }
    }

    for(i = 0; i < sizeof(actions); i++)
    {
        if(actions[i][0] == args)
        {
            isLocated = 1;
            write("Local:  " + actions[i][2] + "\n");
        }
    }

    if(mapp(aliases) && aliases[args])
    {
        isLocated = 1;
        write("Alias: " + args + " -> " + aliases[args] + "\n");
    }

    if(member_array(args, SOUL_D->query_emotes()) != -1)
    {
        isLocated = 1;
        write("Soul: " + args + "\n");
    }


    if(member_array(args + "/t", SOUL_D->query_emotes()) != -1)
    {
        isLocated = 1;
        write("Targetted Soul: " + args + "\n");
    }

    if(environment(previous_object())->valid_exit(args))
    {
        isLocated = 1;
        write("Local Exit: " +
            environment(this_player())->query_exit(args) + "\n");
    }


    if(!isLocated) return notify_fail("%^RED%^Error:%^RESET%^ '" + args
        + "' not found in " +  implode(commandPath, ", ")
         + " nor via a local add_action, alias, soul, or exit.\n");

    return 1;
}

string help(object caller)
{
    return " %^BOLD%^SYNTAX:%^RESET%^ which <verb/command>\n\n"
    "This command will search through your command path, local\n"
    "and global aliases, through local verbs added via add_action,\n"
    "souls, and exits for the specified command (aka verb).\n\n"
    " %^BOLD%^See also:%^RESET%^ path, alias\n";
}
