/* security_editor.c

 Tacitus @ LPUniversity
 14-JAN-06
 Security Editor Object

*/

/* Preproccessor Statements */

#define FILE_GROUPDATA "/adm/etc/groups"
#define FILE_ACCESSDATA "/adm/etc/access"

/* Prototype */

int create_group(string group, string *members) ;
int delete_group(string group) ;
int toggle_membership(string user, string group) ;
int enable_membership(string user, string group) ;
int disable_membership(string user, string group) ;
int set_access(string dir, string id, string *akeys) ;
string *list_groups() ;
string *parse(string str) ;
void parse_files() ;
void parse_group() ;
void parse_access() ;
void write_state(int flag) ;
void write_gFile(int flag) ;
void write_aFile(int flag) ;

void integrity_check() ;

/* Global Variables */

mapping access = ([]) ;
mapping groups = ([]) ;

/* Functions */

void create() {
    if(clonep())
        parse_files() ;
}

string *parse(string str) {
    string *arr ;
    int i ;

    integrity_check() ;

    if(!str) {
        return ({}) ;
    }

    arr = explode(str, "\n") ;

    for(i = 0; i < sizeof(arr); i++) {
        if(arr[i][0] == '#') {
            arr[i] = 0 ;
            continue ;
        }
        arr[i] = replace_string(arr[i], " ", "") ;
        arr[i] = replace_string(arr[i], "\t", "") ;
    }
    return arr ;
}

void parse_files() {
    integrity_check() ;
    parse_group() ;
    parse_access() ;
}

void parse_group() {
    int i, n ;
    string *arr ;

    integrity_check() ;

    arr = parse(read_file(FILE_GROUPDATA)) ;

#ifdef DEBUG

    write_file("/log/security", "\tDebug [security]: Parsing group data file...\n") ;

#endif

    groups = ([]) ;

    for(i = 0; i < sizeof(arr); i++) {
        string group, str, *members ;

        if(!arr[i]) continue ;

        if(sscanf(arr[i], "(%s)%s", group, str) != 2) {
            write("Error [security]: Invalid format of data in group data.\n") ;
            write("Security alert: Ignoring group on line " + (i + 1) + "\n") ;
            continue ;
        }

        members = explode(str, ":") ;

#ifdef DEBUG

        write_file("/log/security", "Debug [security]: Adding group '" + group + "' with " + sizeof(members) + " members.\n") ;

#endif

        for(n = 0; n < sizeof(members); n++) {
            if(!file_size(user_data_file(members[n])) && !sscanf(members[n], "[%*s]")) {
                write("Error [security]: Unknown user detected.\n") ;
                write("Security alert: User '" + members[n] + "' ignored for group '" + group + "'.\n") ;
                members -= ({ members[n] }) ;
                continue ;
            }
#ifdef DEBUG

            write_file("/log/security", "Debug [security]: Adding user '" + members[n] + "' to group '" + group + "'.\n") ;

#endif
        }

        groups += ([group : members]) ;
    }
}

void parse_access() {
    int i, n ;
    string *arr ;

    integrity_check() ;

    arr = parse(read_file(FILE_ACCESSDATA)) ;

#ifdef DEBUG

    write_file("/log/security", "\tDebug [security]: Parsing access data file...\n") ;

#endif

    access = ([]) ;

    for(i = 0; i < sizeof(arr); i++) {
        string directory, str, *entries ;
        mapping data ;

        data = ([]) ;

        if(!arr[i]) continue ;

        if(sscanf(arr[i], "(%s)%s", directory, str) != 2) {
            write("Error [security]: Invalid format of data in access data.\n") ;
            error("Security alert: Fatal error parsing access data on line " + (i + 1) + "\n") ;
        }

        if(str[<1..< 1] == ":") {
            write("Error [security]: Incomplete data in access data (trailing ':').\n") ;
            error("Security alert: Fatal error parsing access data on line " + (i + 1) + "\n") ;
        }

        entries = explode(str, ":") ;

#ifdef DEBUG

        write_file("/log/security", "Debug [security]: Parsing data for directory '" + directory + "'.\n") ;

#endif

        for(n = 0; n < sizeof(entries); n++) {
            string identity, permissions, *perm_array = allocate(8) ;

            if(sscanf(entries[n], "%s[%s]", identity, permissions) != 2) {
                write("Error [security]: Invalid entry(" + n + ") data format in access data.\n") ;
                error("Security alert: Fatal error parsing access data on line " + (i + 1) + "\n") ;
            }

#ifdef DEBUG

            write_file("/log/security", "Debug [security]: Adding identity '" + identity + "' with permission string of '" + permissions + "'.\n") ;

#endif

            //read, write, network, shadow, link, execute, bind, ownership
            if(strsrch(permissions, "r") != -1) perm_array[0] = "r" ;
            if(strsrch(permissions, "w") != -1) perm_array[1] = "w" ;
            if(strsrch(permissions, "n") != -1) perm_array[2] = "n" ;
            if(strsrch(permissions, "s") != -1) perm_array[3] = "s" ;
            if(strsrch(permissions, "l") != -1) perm_array[4] = "l" ;
            if(strsrch(permissions, "e") != -1) perm_array[5] = "e" ;
            if(strsrch(permissions, "b") != -1) perm_array[6] = "b" ;
            if(strsrch(permissions, "o") != -1) perm_array[7] = "o" ;

            data += ([ identity : perm_array ]) ;

        }

        access += ([directory : data]) ;
    }
}

int create_group(string group, string *members) {
integrity_check() ;

    if(groups[group])
        return 0 ;

    if(!sizeof(members))
        return 0 ;

    groups += ([group : members]) ;

    return 1 ;
}

int delete_group(string group) {
integrity_check() ;

    if(!groups[group]) return 0 ;
    map_delete(groups, group) ;
    return 1 ;
}

int enable_membership(string user, string group) {
    string *user_list ;

    integrity_check() ;

    if(!groups[group])
        return 0 ;

    user_list = groups[group] ;

    if(member_array(user, user_list) == -1)
        groups[group] += ({user}) ;
    else
        return 0 ;

    return 1 ;
}

int disable_membership(string user, string group) {
    string *user_list ;

    integrity_check() ;

    if(!groups[group]) return 0 ;
    user_list = groups[group] ;

    if(member_array(user, user_list) != -1)
        groups[group] -= ({user}) ;
    else
        return 0 ;

    return 1 ;
}

int toggle_membership(string user, string group) {
    string *user_list ;

    integrity_check() ;

    if(!groups[group]) return 0 ;
    user_list = groups[group] ;

    if(member_array(user, user_list) == -1)
        groups[group] += ({user}) ;
    else
        groups[group] -= ({user}) ;

    return 1 ;
}

int set_access(string dir, string id, string *akeys) {
    integrity_check() ;

    if(!sizeof(akeys)) return 0 ;

    if(!access[dir]) access += ([dir : ([id : akeys]) ]) ;
    else access[dir] += ([ id : akeys]) ;
    return 1 ;
}

string *list_groups() {
    string *keys ;

    integrity_check() ;

    keys = keys(groups) ;
    return keys ;
}

void write_state(int flag) {
    integrity_check() ;

    if(!adminp(previous_object()) && !adminp(this_body()))
        return ;

    write_gFile(flag) ;
    write_aFile(flag) ;
}

void write_gFile(int flag) {
    string file = "" ;
    string *group_list = keys(groups), *group_data ;
    int i ;
    string err = "" ;

    integrity_check() ;

    if(!adminp(previous_object()) && !adminp(this_body()))
        return ;

    i = 0 ;
    group_list = keys(groups) ;
    group_data = ({}) ;
    file = "" ;


    for(i = 0; i < sizeof(group_list); i ++) {
        group_data = groups[group_list[i]] ;
        file += "(" + group_list[i] + ")" ;
        if(sizeof(group_data) > 1) file += sprintf("%s%s\n",implode(group_data[0..(sizeof(group_data)-2)], ":"), ":" + group_data[sizeof(group_data)-1]) ;
        else if(sizeof(group_data) == 1) file += group_data[0] + "\n" ;
        else error("ERROR: Group '" + group_list[i] + "' has no members!") ;
    }

    if(flag)
        write(file + "\n") ;
    else {
        write_file("/adm/etc/groups", file, 1) ;
        parse_files() ;
        err += catch(destruct(master())) ;
        err += catch(destruct(find_object("/adm/obj/master/valid"))) ;
        err += catch(load_object("/adm/obj/master/valid")) ;
        err += catch(load_object("/adm/obj/master")) ;
        err += catch(CONFIG_D->rehash_config()) ;
        if(err != "00000") write(err) ;
    }
}

void write_aFile(int flag) {
    string file = "" ;
    string *access_list = keys(access), *keys, *arr = ({}) ;
    mapping access_data ;
    int i, j ;
    string err = "" ;

    integrity_check() ;

    if(!adminp(previous_object()) && !adminp(this_body()))
        return ;

    i = 0 ;
    j = 0 ;
    access_data = ([]) ;
    access_list = keys(access) ;
    keys = ({}) ;
    arr = ({}) ;
    file = "" ;

    for(i = 0; i < sizeof(access_list); i++) {
        access_data = access[access_list[i]] ;
        file += "(" + access_list[i] + ") " ;
        keys = keys(access_data) ;
        arr = ({}) ;
        for(j = 0; j < sizeof(keys); j ++)
        arr += ({(sprintf("%s[%s]", keys[j], implode(access_data[keys[j]], "")))}) ;
        if(sizeof(arr) > 1) file += sprintf("%s%s\n",implode(arr[0..(sizeof(arr)-2)], ":"), ":" + arr[sizeof(arr)-1]) ;
        else file += arr[0] + "\n" ;
    }

    if(flag)
        write(file + "\n") ;
    else {
        write_file("/adm/etc/access", file, 1) ;
        parse_files() ;
        err += catch(destruct(master())) ;
        err += catch(destruct(find_object("/adm/obj/master/valid"))) ;
        err += catch(load_object("/adm/obj/master/valid")) ;
        err += catch(load_object("/adm/obj/master")) ;
        err += catch(CONFIG_D->rehash_config()) ;
        if(err != "00000") write(err) ;
    }
}

void integrity_check() {
    if(!clonep())
        error("Error [security_editor]: This object must be cloned to be used.\n") ;
}
