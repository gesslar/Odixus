/*login.c

* Tacitus @ LPUniversity
* 04-APR-05
* Login Daemon

* Recoded on October 7th, 2005 by Tacitus

*/

/* Last edited on October 6th, 2006 by Tacitus */

#include <config.h>
#include <logs.h>

/* #define EMAIL_MUST_RESOLVE */

void get_name(string str);
void get_password(string str, int i);
void verify_password(string str, int i);
void new_user(string str, string name);
void auto_destruct();
void reconnect(string str);
void setupNew();
void enterWorld(string str);
void email_verified(string address, string resolved, int key);
void get_email(string input);
void idle_email(string str);
object create_body(string name);
string parseTokens(string text);

object user;
object body;

string read = "\n" + parseTokens(read_file(LOGIN_NEWS));
string loginMsg = parseTokens(read_file(LOGIN_MSG));
string email;

int isConnected = 0, coID;

void create() {
    if(clonep(this_object())) coID = call_out("auto_destruct", 60);
}

void logon() {
    write(loginMsg);
    write("\nPlease select a name: ");
    input_to("get_name");
}

void get_name(string str) {
    int i;
    string err;

    load_object(LOCKDOWN_D);

    write("\n");

    if(LOCKDOWN_D->is_ip_banned(query_ip_number(this_object()))) {
        write("\nYour IP address, " + query_ip_number(this_object()) + " has been banned from " + mud_name() + ".\n");
        destruct() ;
        return;
    }

    if(!str || strlen(str) < 2) {
        write("You must select a name: ");
        input_to("get_name");
        return;
    }

    if(str == "quit") {
        write("Come back soon!\n");
        destruct() ;
        return;
    }

    if (strlen(str) > 11) {
        write("Sorry, your name can't have more than 11 characters.\n");
        write("Please select a name: ");
        input_to("get_name");
        return;
    }

    str = lower_case(str);

    for (i = 0; i < strlen(str); i++) {
        if (str[i] < 'a' || str[i] > 'z') {
            write("Sorry, your name can only have letters. (a-z)\n"
                "Please enter a new name: ");
            input_to("get_name");
            return;
        }
    }

    err = catch(user = new(USER_OB));

    if(err) {
        write("Error [login]: Unable to create user object.\n");
        write("It appears that the login object is dysfunctional. Try again later.\n");
        write("Error: " + err + "\n");
        destruct() ;
        return;
    }

    if(LOCKDOWN_D->is_user_banned(str)) {
        write("\nThe username " + capitalize(str) + " is banned from " + mud_name() + ".\n");
        destruct() ;
        return;
    }

    if(LOCKDOWN_D->query_dev_lock() && wizardp(str) && !adminp(str)) {
        write("\n" + LOCKDOWN_D->query_dev_lock_msg() + "\n");
        destruct() ;
        return;
    }

    if(LOCKDOWN_D->query_player_lock() && (!adminp(str) && !wizardp(str))) {
        write("\n" + LOCKDOWN_D->query_player_lock_msg() + "\n");
        destruct() ;
        return;
    }

    if(LOCKDOWN_D->query_vip_lock() && (!adminp(str) && !wizardp(str) && (member_array(str, LOCKDOWN_D->query_play_testers()) == -1))) {
        write("\n" + LOCKDOWN_D->query_vip_lock_msg() + "\n");
        destruct() ;
        return;
    }

    if(str == "guest") {
        if(LOCKDOWN_D->query_guest_locked()) {
            write("\n" + LOCKDOWN_D->query_guest_lock_msg() + "\n");
            destruct() ;
            return;
        }

        user->set_name("guest");
        set_privs(user, "guest");
        body = create_body("guest");
        write_file(LOG_DIR + "/" + LOG_LOGIN, capitalize(user->query_name()) + " logged in from " +
          query_ip_number(body) + " on " + ctime(time()) + "\n");
        write(read + "\n");
        write(" [Hit enter to continue] ");
        input_to("enterWorld");
        return;
    }

    if (!file_exists(user_data_file(str) + ".o")) {
        if(LOCKDOWN_D->query_player_lock()) {
            write("\n" + LOCKDOWN_D->query_player_lock_msg() + "\n");
            destruct() ;
            return;
        }

        write("The user " + str + " does not exist. Would you like to create it? ");
        input_to("new_user", str);
        return;
    }

    set_privs(user, str);
    user->set_name(str);
    user->restore_user();

    if(!user->query_password() || user->query_password() == "") {
        write("Your account has no password. All accounts must have a password.\n");
        write("Please enter a new password: ");
        input_to("get_password", 1, 2);
        return;
    }

    write("Please enter your password: ");
    input_to("get_password", 1, 0);

    return;

}

void get_password(string str, int i) {
    string pass;

    if(!i) {
        pass = crypt(str, str);
        if(pass != user->query_password()) {
            write("That password is incorrect.\n");
            write("Please enter your password: ");
            input_to("get_password", 1, 0);
            return;
        } else {
            isConnected = 1;
            if(find_player(user->query_name())) {
                write("\nReconnect to currently logged in body? ");
                input_to("reconnect");
                return;
            }
            write_file(LOG_DIR + "/" + LOG_LOGIN, capitalize(user->query_name()) + " logged in from " +
              query_ip_number(this_object()) + " on " + ctime(time()) + "\n");
            write(read + "\n");
            write(" [%^BOLD%^Hit enter to continue%^RESET%^] ");
            input_to("enterWorld");
            return;
        }
    } else {
        if(!str) {
            write("Your user must have a password.\n");
            write("Please enter a password: ");
            input_to("get_password", 1, 0);
            return;
        }

        user->set_password(str);
        write("\nPlease enter your password again to verify: ");
        input_to("verify_password", 1, i);
        return;
    }
}

void verify_password(string str, int i) {
    str = crypt(str, str);
    if(str != user->query_password()) {
        write("Your passwords do not match.\n");
        write("Please enter a password: ");
        input_to("get_password", 1, 1);
        return;
    } else if(i != 2) {
        write("\nPlease input your public e-mail address: ");
        input_to("get_email");
        return;
    } else {
        write("\nYou may now login with the password you just set.\n");
        write("Please enter your password: ");
        input_to("get_password", 1, 0);
        return;

    }

}

void get_email(string input) {
    string host;

    if(!input) {
        write("Error [login]: You must provide an e-mail address.\n");
        write("Please input your e-mail address: ");
        input_to("get_email");
        return;
    }

    if(!sscanf(input, "%*s@%s", host)) {
        write("Error [login]: You must provide a valid e-mail address.\n");
        write("Please input your e-mail address: ");
        input_to("get_email");
        return;
    }

    if(host == "localhost") {
        write("Error [login]: Please provide a different e-mail address.\n");
        write("Please input your e-mail address: ");
        input_to("get_email");
        return;
    }

    email = input;

    resolve(host, "email_verified");
    write("\n Please hit any key to continue.\n");
    input_to("idle_email");
}

void idle_email(string str) {
    object security_editor;

#ifdef EMAIL_MUST_RESOLVE
    if(!isConnected) {
        write("Error [login]: Unable to verify e-mail. Press press any key to try again.\n");
        input_to("idle_email");
        return;
    } else {
#endif
        set_privs(user, user->query_name());
        body = create_body(query_privs(user));

        if(file_exists("/adm/etc/new_install")) {
            if(!directory_exists("/home/" + user->query_name()[0..0])) mkdir ("/home/" + user->query_name()[0..0]);
            mkdir("/home/" + user->query_name()[0..0] + "/" + user->query_name());
            mkdir("/home/" + user->query_name()[0..0] + "/" + user->query_name() + "/public");
            mkdir("/home/" + user->query_name()[0..0] + "/" + user->query_name() + "/private");
            catch(cp("/areas/std/workroom.c", user_path(user->query_name())));
            write_file("/home/" + user->query_name()[0..0] + "/" + user->query_name() + "/private/journal." + user->query_name(), capitalize(user->query_name()) + "'s dev journal (Created: " + ctime(time()) + ")\n\n");
            catch(link("/home/" + user->query_name()[0..0] + "/" + user->query_name() + "/private/journal." + user->query_name(), "/doc/journals/journal." + user->query_name()));
            body->add_path("/cmds/wiz/");
            body->add_path("/cmds/object/");
            body->add_path("/cmds/file/");
            body->add_path("/cmds/adm/");
            security_editor = clone_object("/adm/obj/security_editor.c");
            security_editor->enable_membership(query_privs(user), "developer");
            security_editor->enable_membership(query_privs(user), "admin");
            security_editor->write_state(0);
            rm("/adm/etc/new_install");
            write("\t\tSuccess [login]: You are now an admin.\n\n");
        }

        user->set("email", email);
        write_file(LOG_DIR + "/" + LOG_NEWUSER, capitalize(user->query_name()) + " created a new account from "
          + query_ip_number(this_object()) + " on " + ctime(time()) + "\n");
        write_file(LOG_DIR + "/" + LOG_LOGIN, capitalize(user->query_name()) + " logged in from " +
          query_ip_number(this_object()) + " on " + ctime(time()) + " for the first time.\n");
        write("\n" + parseTokens(read_file(FLOGIN_NEWS)) + "\n");
        write(" [%^BOLD%^Hit enter to continue%^RESET%^] ");
        input_to("setupNew");
#ifdef EMAIL_MUST_RESOLVE
    }
#endif
}

void email_verified(string address, string resolved, int key)
{
#ifdef EMAIL_MUST_RESOLVE
    if(!resolved) {
        write(" Error [login]: Unable to verify e-mail.\n");
        write("Please input your e-mail address: ");
        input_to("get_email");
        return;
    }
#endif
    isConnected = 1;
}

void new_user(string str, string name) {
    if(str == "y" || str == "yes" || str == "yup" || str == "sure" || str == "indeed") {
        write("Please enter a password for your account: ");
        user->set_name(name);
        input_to("get_password", 1, 1);
        return;
    }

    write("Please select another name then: ");
    input_to("get_name");
}

void setupNew() {
    if(!directory_exists(user_data_directory(user->query_name()))) mkdir(user_data_directory(user->query_name()));
    user->save_user();
    if(objectp(body)) body->save_user();
    write(read + "\n");
    write("\n");
    write(" [%^BOLD%^Hit enter to continue%^RESET%^] ");
    input_to("enterWorld");
}

void reconnect(string str) {
    object oldBody = find_player(user->query_name());

    if(str == "y" || str == "yes" || str == "yup" || str == "sure" || str == "indeed") {
        tell_object(oldBody, "Warning: Your body has been displaced by another user.\n");
        write_file(LOG_DIR + "/" + LOG_LOGIN, capitalize(oldBody->query_name()) + " reconnected from " +
          query_ip_number(this_object()) + " on " + ctime(time()) + "\n");
        if(interactive(oldBody)) remove_interactive(oldBody);
        oldBody->reconnect();
        write(read + "\n");
        write(" [%^BOLD%^Hit enter to continue%^RESET%^] ");
        body = oldBody;
        input_to("enterWorld");
        return;
    } else {
        write_file(LOG_DIR + "/" + LOG_LOGIN, capitalize(user->query_name()) + " logged in from " +
          query_ip_number(user) + " on " + ctime(time()) + "\n");
        write(read + "\n");
        write(" [Hit enter to continue] ");
        input_to("enterWorld");
        return;
    }

}

void auto_destruct() {
    if(isConnected) return;
    if(interactive(this_object())) write("\n\nNotice: I'm sorry, but you are only allowed 60 seconds to attempt to login.\n");
    destruct() ;
}

void enterWorld(string str) {
    object *obs;

    obs = users();

    if(!objectp(body)) body = create_body(query_privs(user));

    exec(body, this_object());
    body->setup();
    body->set_link(user);
    user->set_body(body);

    if(devp(user)) {
        if(body->query_env("start_location") == "last_location") body->move(body->query("last_location"));
        else if (body->query_env("start_location")) body->move(body->query_env("start_location"));
        else if(file_exists(user_path(query_privs(body)) + "workroom.c")) body->move(user_path(query_privs(user)) + "workroom.c");
    }

    else body->move("/areas/lpuni/entrance.c");

    if(!environment(body)) body->move("/areas/lpuni/entrance.c");

    body->enter_world();
    remove_call_out(coID);

    destruct() ;
}

void catch_tell(string message) {
    receive(ANSI_PARSER->parse_pinkfish(message));
}

void relogin() {
    object login = new(LOGIN_OB);
    object user = this_player();

    user->exit_world();
    log_file(LOG_LOGIN, capitalize(user->query_name()) + " logged out from " +
      query_ip_number(user) + " on " + ctime(time()) + "\n");
    exec(login, user);
    user->save_user();
    user->remove();
    login->logon();
}

object create_body(string name) {
    string err;

    if(origin() != "local") return 0;

    err = catch(body = new(user->query_bodyPath()));

    if(err) {
        error("Error [login]: There was an error creating your mobile.\n" +
          "\tBody: " + user->query_bodyPath() + "\n");
    }

    body->set_name(query_privs(user));
    user->set_body(body);
    set_privs(body, body->query_name());
    body->restore_user();

    return body;
}

string query_name() {
    return "login";
}

string parseTokens(string text) {
    catch {
        text = replace_string(text, "%mud_name", mud_name());
        text = replace_string(text, "%users", implode(
            filter(users()->query_cap_name(), (: $1 != "login" :))[0..<2], ", ") +
            ", and " + filter(users()->query_cap_name(),
            (: $1 != "login" :) )[<1]);
        text = replace_string(text, "%user_count", "" + sizeof(users()));
        text = replace_string(text, "%date", ctime(time()));
        text = replace_string(text, "%open_status", OPEN_STATUS);
        text = replace_string(text, "%email", ADMIN_EMAIL);
        text = replace_string(text, "%lib_name", lib_name());
        text = replace_string(text, "%lib_version", lib_version());
        text = replace_string(text, "%cap_mud_name",
            implode(explode(mud_name(), ""), (: capitalize($1)
            + capitalize($2) :)));
    };

    return text;
}

void receive_message(string type, string msg) {
    msg = ANSI_PARSER->parse_pinkfish(msg, 1);
    receive(msg);
}
