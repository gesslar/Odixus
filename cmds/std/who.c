//who.c

//Tacitus @ LPUniversity
//08-APR-05
//Standard Command

//I don't like it, please recode this for me :) [Tacitus]

//Last edited October 4th, 2006 by Tacitus

object *addArray( object *oldlist, object *newlist);

int main(string arg)
{
    string ret, mudname;
    mixed *packet;
    object *list, c;
    object *oAdminArr,*oDevArr, *oUserArr;
    int i;


    oAdminArr = ({});
    oDevArr   = ({});
    oUserArr  = ({});
    ret = "";

    /* Fixed your error. Tricky */

    if(arg && sscanf(arg, "@%s", mudname) == 1)
    {
        c = load_object("/adm/daemons/chmodules/chdmod_i3.c");
        packet = ({ "who-req", 5, mud_name(), this_player()->query_name(), mudname, 0 });
        c->send_packet(packet);
        return 1;
    }

    ret +=
    "\t%^BOLD%^%^WHITE%^ _       _____   _    _         %^RESET%^RED%^_%^RESET%^BOLD%^WHITE%^ \n%^RESET%^"
    "\t%^BOLD%^%^WHITE%^| |     |  __ \\ | |  | |       (%^RESET%^RED%^_%^RESET%^BOLD%^WHITE%^)\n%^RESET%^"
    "\t%^BOLD%^%^WHITE%^| |     | |__) || |  | | _ __   %^RESET%^BLUE%^_%^RESET%^BOLD%^WHITE%^ \n%^RESET%^"
    "\t%^BOLD%^%^BLUE%^| |     %^BOLD%^%^WHITE%^|  ___/ | |  | || '_ \\ | |\n%^RESET%^"
    "\t%^BOLD%^%^BLUE%^| |____ | |     | |__| || | | || |\n%^RESET%^"
    "\t%^BOLD%^%^BLUE%^|______||_|      \\____/ |_| |_|%^BOLD%^%^WHITE%^|%^RESET%^%^BLUE%^_%^BOLD%^WHITE%^|\n\n\n%^RESET%^";



    ret += sprintf("%10s  %10s\n\n", "Username [* editing, + in input]", "Idle");

    list = users();

    foreach( object name in list )
    {
        if ( adminp( name ) && name->query_name() != "login" )
            oAdminArr += ({ name });
        else if ( devp( name ) )
            oDevArr += ({ name });
        else
            oUserArr += ({ name });

    }
    
    oAdminArr = sort_array( oAdminArr, "sort_name");
    oDevArr   = sort_array( oDevArr,   "sort_name");
    oUserArr = sort_array( oUserArr, "sort_name");

    list = ({});
    list = addArray( list, oAdminArr );
    list = addArray( list, oDevArr );
    list = addArray( list, oUserArr );

    //    list = sort_array(list, "sort_users");

    for (i = 0; i < sizeof(list); i ++)
    {
        string tag;

        if( !(string)list[i]->query_name()) 
            continue;

        if(list[i]->query_name() == "login")
            tag = "[ %^BOLD%^CYAN%^LOGIN%^RESET%^ ]";
        else if(adminp(list[i])) 
            tag = "[ %^BOLD%^RED%^Admin%^RESET%^ ]";
        else if(devp(list[i])) 
            tag = "[ %^YELLOW%^Dev%^RESET%^   ]";
        else 
            tag = "[ %^GREEN%^User%^RESET%^  ]";

        ret += sprintf(" %-s   %-15s %15s\n", tag,
          capitalize((string)list[i]->query_name()) +
          (list[i]->query_env("away") ? " (afk)" : "") +
          (in_edit( list[i] ) ? "*" : "") +
          (in_input( list[i] ) && !list[i]->query_env("away") ? "+" : ""),
          query_idle( list[i] ) / 60 + "m");
    }

    ret +=("\n");
    write(ret);
    return 1;
}

int sort_name(object ob1, object ob2)
{
    if ( ob1->query_name() > ob2->query_name() )    return 1;
    else if ( ob1->query_name() < ob2 ->query_name() )    return -1;
    else return 0;
}

object *addArray(object *oldarr, object *newarr )
{
    foreach( object name in newarr )
    {
        oldarr += ({ name });
    }

    return oldarr;
}

string help()
{
    return(HIW + " SYNTAX: " + NOR + "who [@<mud>]\n\n" +
      "This command will display all the users who are currently logged\n" +
      "into " + mud_name() + ". It also lets you know if they are currently\n" +
      "editing, in input, and/or idle. You may also find out who is on a mud\n" +
      "on the I3 network by using the 'who @<mud>' syntax\n");
}

