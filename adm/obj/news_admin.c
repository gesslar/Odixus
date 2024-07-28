/*
     Tacitus @ LPUniversity
     26-FEB-06
     Admin Object
*/

// Last updated on August 10th, 2006 by Parthenon@LPUniversity

inherit STD_ITEM;

/* Function Prototypes */

int mm_select(string arg);
int create_group(string arg);
int delete_group(string arg);
int delete_message(string group);
int purge_group(string args);
int add_client(string args);
int del_client(string args);
int delete_message_final(string id, string group);

object s_editor;

/* Functions */

void create()
{
    s_editor = new(OBJ_SECURITY_EDITOR);
}

int main_menu()
{
    if(!adminp(this_body())) return 0;

    write("\n\t-= NEWS SYSTEM ADMINISTRATION =-\n\n");
    write(
      "1. Create a new news group.\n"
      "2. Delete a news group.\n"
      "3. Delete a post in a news group.\n"
      "4. Purge a news group.\n"
      "5. Authorize a news client.\n"
      "6. Unauthorize an authorized news client.\n"
      "7. News system permissions.\n"
      "8. Quit\n\n");

    write("Main Menu > ");
    input_to("mm_select");
    return 1;
}

int mm_select(string arg)
{
    int select;

    select = to_int(arg);
    if(!intp(select)) return main_menu();

    switch(select)
    {
    case 1 :
    {
        write("\nName of new group: ");
        input_to("create_group");
        return 1;
    }

    case 2 :
    {
        write("\nName of group to delete: ");
        input_to("delete_group");
        return 1;
    }

    case 3 :
    {
        write("\nName of group where message exists: ");
        input_to("delete_message");
        return 1;
    }

    case 4 :
    {
        write("\nName of group to purge: ");
        input_to("purge_group");
        return 1;
    }

    case 5 :
    {
        write("\nFull path to client: ");
        input_to("add_client");
        return 1;
    }

    case 6 :
    {
        write("\nFull path to client: ");
        input_to("del_client");
        return 1;
    }

    case 7 :
    {
        write("\nName of news group to set permissions for: ");
        input_to("set_permissions");
        return 1;
    }

    case 8 :
    {
        write("\nExiting news daemon admin interface...\n");
        remove() ;
        return 1;
    }

    default : return main_menu();
    }

}

int set_permissions(string arg)
{
    if(!arg || arg == "") return main_menu();

    if(!NEWS_D->group_exists(capitalize(arg)))
    {
    write("\nError [news admin]: news group does not exist\n");
    return main_menu();
    }

    write("\nUser group you wish to edit access for: ");
    input_to("select_user_group", 0, arg);
    return 1;
}

int select_user_group(string arg, string news_group)
{
    if(!arg || arg == "")
    {
    write("\nError [news admin]: no user group selected\n");
    return main_menu();
    }

    if(member_array(arg, s_editor->list_groups()) == -1)
    {
    write("\nError [news admin]: user group does not exist\n");
    return main_menu();
    }

    write("\nPermissions to give user group " + arg + " ('o' to receieve a list of options): ");
    input_to("select_permissions", 0, news_group, arg);
    return 1;
}

int select_permissions(string arg, string news_group, string user_group)
{
    string options = "";

    if(!arg || arg == "")
    {
    write("\nError [news admin]: incorrect options\n");
    return main_menu();
    }

    if(arg == "h" || arg == "o")
    {
    write("r - read access\n");
    write("p - post access\n");
    write("e - edit access\n");
    write("If you wish to give multiple options just enter them as a single string. Ex. 'rp' or 'rpe'\n"+
      "Enter 'none' if you wish the user group to have no permissions.\n");
    write("\nPermissions to give user group " + user_group + " ('o' to receieve a list of options): ");
    input_to("select_permissions", 0, news_group, user_group);
    return 1;
    }

    if(strsrch(arg, "r") != -1)
    options += "r";

    if(strsrch(arg, "p") != -1)
    options += "p";

    if(strsrch(arg, "e") != -1)
    options += "e";

    if(arg == "none")
    options = "";

    write(NEWS_D->admin_action_set_permissions(news_group, user_group, options) ?
      "\nSuccess [news admin]: permissions for " + user_group + " in group " + news_group + " set to '" + options + "'.\n" :
      "\nError [news admin]: could not set permissions for " + user_group + " in group " + news_group + " to '" + options + "'.\n");

    return main_menu();
}

int create_group(string arg)
{
    if(!stringp(arg)) return main_menu();
    write(NEWS_D->admin_action_create_group(arg) ? "\nSuccess.\n" : "\nFailure.\n");
    return main_menu();
}

int delete_group(string arg)
{
    if(!stringp(arg)) return main_menu();
    write(NEWS_D->admin_action_delete_group(arg) ? "\nSuccess.\n" : "\nFailure.\n");
    return main_menu();
}

int delete_message(string group)
{
    write("\nEnter id of message: ");
    input_to("delete_message_final", 0, group);
    return 1;
}

int delete_message_final(string id, string group)
{
    int numerical_id = to_int(id);
    if(!intp(numerical_id))
    {
    write("\nInvalid Input\n");
    return main_menu();
    }

    write(NEWS_D->admin_action_delete_post(group, numerical_id) ? "\nSuccess.\n" : "\nFailure.\n");

    return main_menu();
}

int purge_group(string args)
{
    write( NEWS_D->admin_action_delete_group(args) && NEWS_D->admin_action_create_group(args) ? "\nSuccess.\n" : "\nFailure.\n");

    return main_menu();
}

int add_client(string args)
{
    write( NEWS_D->authorize_client(args) ? "\nSuccess.\n" : "\nFailure.\n");

    return main_menu();
}

int del_client(string args)
{
    write( NEWS_D->revoke_client_authorization(args) ? "\nSuccess.\n" : "\nFailure.n");

    return main_menu();
}

int remove() {
    if(s_editor)
        destruct(s_editor);
    return ::remove() ;
}
