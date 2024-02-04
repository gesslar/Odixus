/* finger_d.c

 Tacitus @ LPUniversity
 26-MAR-06
 Finger Daemon

*/

//Last edited on October 6th by Tacitus

/*

 The finger daemon provides an interface to load user objects
 to gather information.

 */

#include <socket.h>

inherit STD_DAEMON ;

#define BORDER1 "=+=--=+=--=+=--=+=--=+=--=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=\n"

mixed get_user(string username);
mixed get_body(object user);

varargs string finger_user(string username)
{
    string ret, group, *users;
    string rank, last_t, last, idle, plan, away;
    int idle_time;
    mixed user, body;
    mixed *packet;

    username = lower_case(username);

    if(sscanf(username, "(%s)", group))
    {
        users = master()->query_group(group);

        if(!pointerp(users) || sizeof(users) <= 0)
            return "Error: Group '" + group
             + "' does not exist or has no members.\n";

        ret = BORDER1;
        ret += "\t\tGROUP: " + group + "\n";
        ret += BORDER1;
        ret += sprintf("%-20s %-40s %s\n", "Login", "Rank", "On");
        ret += BORDER1;

        foreach(username in users)
        {
            if(sscanf(username, "(%*s)"))
                ret += sprintf("%-20s %-40s %s\n", username, "(GROUP)", "---");
            else
            {

                 if(find_player(username))
                 {
                     last_t = "On Since";
                     body = find_player(username);
                     last = ctime(body->query("last_login"));
                 }

                 else
                 {
                     if(!objectp(body = get_body(get_user(username)))) continue;

                     else
                     {
                         last_t = "Last on";
                         last =  ctime(body->query("last_login"));
                         destruct(body);
                     }

                 }


                 if(adminp(username)) rank = "Admin";
                 else if(devp(username)) rank = "Developer";
                 else rank = "User";

                 ret += sprintf("%-20s %-40s %s\n",
                     capitalize(username), rank, last_t + " " + last);
            }
        }

        return ret += BORDER1;

    }

    else
    {

        if(find_player(username))
        {
            last_t = "On since";
            body = find_player(username);
            user = get_user(username);
            if(!interactive(body))
                idle = "(Link-Dead)";
            else
            {
                away = find_player(username)->query_env("away");

                if(query_idle(body))
                {
                    idle_time = query_idle(body)/60;
                    if(idle_time > 0)
                        idle = "(Idle: " + idle_time + ")";
                    else
                        idle = "";
                }
                else idle = "";
            }
        }

        else
        {
            last_t = "Last on";
            idle = "(Offline)";
            if(!objectp(user = get_user(username)) || !objectp(body = get_body(user)))
            {
                if(user == -2) return "Error [finger]: That user doesn't exist.\n";
                else return "Error [finger]: User data unavailable.\n";
            }
        }

        last = ctime(body->query("last_login"));

        if(adminp(user)) rank = "Admin";
         else if(devp(user)) rank = "Developer";
         else rank = "User";

         plan = read_file("/home/" + user->query_name()[0..0] + "/" + user->query_name() + "/.plan");
         if(!plan) plan = " This user has no plan.\n";



        ret = sprintf(
        "\n"
        "Username: %-10s \tRank: %-10s\n" + (away && away != "" ? "Away: " + away + "\n" : away == "" ? "This user is away.\n" : "") +
        "E-mail: %-10s\n"
        "%s: %-10s %s\n"
        "Plan:\n%s", capitalize(user->query_name()) + "",
        rank, user->query("email"), last_t, last, idle, plan);

        if(!interactive(user)) destruct(user);
    }
    return ret;

}

mixed get_user(string username)
{
    object user;
    string error;

    if(origin() != "local" && !adminp(query_privs(previous_object()))) return 0;

    error = catch(user = new(USER_OB));
    if(error) return -1;
    if(!file_exists(user_data_file(username) + ".o")) return -2;

     set_privs(user, username);
     user->set_name(username);
     user->restore_user();

     return user;
}

mixed get_body(object user)
{
    object body;
    string error;

    if(origin() != "local" && !adminp(query_privs(previous_object()))) return 0;

    error = catch(body = new(user->query_bodyPath()));
    if(error) return -1;
    if(!file_exists(user_mob_data(query_privs(user)) + ".o")) return -2;

     set_privs(body, query_privs(user));
     body->set_name(user->query_name());
     body->restore_user();

     return body;
}
