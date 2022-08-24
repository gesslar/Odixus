/* news_d.c

 Tacitus @ LPUniversity
 27-JAN-06
 News Daemon
*/

// Last edited by Parthenon on August 10th, 2006

/* Preproccessor Statements */

inherit DAEMON ;

#define DATA_FILE "/data/daemons/news_d"

/* Global Variables */

mapping data;
mapping permissions;
string *authorizedClients;

/* Functions */

void create()
{
    int i, k;
    string *groups;
    object s_editor;

    s_editor = clone_object("/adm/obj/security_editor.c");
    groups = s_editor->listGroups();
    destruct(s_editor);

    permissions = ([]);
    data = ([]);
    authorizedClients = ({ "/obj/mudlib/newsclients/std_newsclient" });
    restore_object(DATA_FILE);
    if(!permissions) permissions = ([]);
    if(!data) data = ([]);

    for(i = 0; i < sizeof(keys(data)); i++)
    {
    if(!permissions[keys(data)[i]])
        permissions[keys(data)[i]] = ([]);

    for(k = 0; k < sizeof(groups); k++)
        if(!permissions[keys(data)[i]][groups[k]])
        permissions[keys(data)[i]][groups[k]] = "r";
    }

    save_object(DATA_FILE);
}

int authorizeClient(string filename)
{
    if(!adminp(this_player())) return 0;

    if(!file_exists(filename)) return 0;

    if(member_array(base_name(previous_object()), authorizedClients) != -1) return 0;

    authorizedClients += ({ filename });

    return 1;
}

int revokeClientAuthorization(string filename)
{
    if(!adminp(this_player())) return 0;

    if(member_array(base_name(previous_object()), authorizedClients) == -1) return 0;

    authorizedClients -= ({ filename });

    return 1;
}

int clientAction_edit(string group, string contents, int index)
{
    if(member_array(base_name(previous_object()), authorizedClients) == -1) return 0;

    if(!data[group]) return 0;

    data[group][index]["content"] = contents + "\n\nEDITIED ON " + upper_case(ctime(time()));

    save_object(DATA_FILE);

    return 1;
}

int clientAction_post(string group, string author, string subject, string content)
{
    mapping new_msg;

    if(member_array(base_name(previous_object()), authorizedClients) == -1) return 0;

    if(!data[group]) return 0;

    new_msg = ([
      "group" : group,
      "author" : author,
      "subject" : subject,
      "content" : content,
      "date" : time(),
      "id" : sizeof(data[group]) + 1,
    ]);

    data[group] += ({ new_msg });

    save_object(DATA_FILE);

    return 1;
}

mapping *getPosts(string group)
{
    mapping *ret;

    if(member_array(base_name(previous_object()), authorizedClients) == -1) return 0;

    if(!data[group]) return 0;

    ret = data[group];

    return ret;
}

string *getGroupListings()
{
    if(member_array(base_name(previous_object()), authorizedClients) == -1) return 0;

    return keys(data);
}


int adminAction_deletePost(string group, int id)
{
    if(!adminp(this_player())) return 0;

    if(!data[group]) return 0;

    if(!data[group][id]) return 0;

    map_delete(data[group], id);

    return 1;
}

int adminAction_createGroup(string group)
{
    if(!adminp(this_player())) return 0;

    if(data[group]) return 0;
    data += ([group : ({})]);

    save_object(DATA_FILE);

    return 1;
}

int adminAction_deleteGroup(string group)
{
    if(!adminp(this_player())) return 0;

    if(!data[group]) return 0;
    map_delete(data, group);

    save_object(DATA_FILE);

    return 1;
}

int adminAction_setPermissions(string news_group, string user_group, string options)
{
    if(!adminp(this_player())) return 0;
    if(!data[capitalize(news_group)]) return 0;

    news_group = capitalize(news_group);

    if(!permissions[news_group]) permissions[news_group] = ([]);

    permissions[news_group][user_group] = options;

    save_object(DATA_FILE);

    return 1;
}

int groupExists(string group)
{
    if(data[group]) return 1;
    else return 0;
}

int has_permission(string news_group, string action)
{
    int i = 0;
    string access, group;

    if(adminp(this_player())) return 1;

    for(i = 0; i < sizeof(permissions[news_group]); i++)
    {
    access = permissions[news_group][keys(permissions[news_group])[i]];
    group = keys(permissions[news_group])[i];

    if(isMember(query_privs(this_player()), group) && strsrch(access, action) != -1)
        return 1;
    }

    return 0;
}


