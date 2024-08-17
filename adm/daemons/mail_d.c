/*

 mail_d.c

 Parthenon@LPUniversity
 July 25th, 2006
 the mail daemon that handles all of the sending/receiving of mail messages

 Last edited on August 17th, 2006 by Parthenon

*/

inherit STD_DAEMON ;

#define MAIL_DIR "/data/users/"

mapping inbox = ([]);
mapping outbox = ([]);
int curr_in_msg, curr_out_msg;
int in_end_index, in_start_index, out_end_index, out_start_index;
nosave object s_editor;

string *get_all_members(string *parent_users);

string get_mail_box_file(string user) {
    return MAIL_DIR + user[0..0] + "/" + user + "/" + user + ".mail.o";
}//END get_mail_box_file

mixed send_message(mapping mail, string owner, int in_msg, int out_msg) {
    int i, num, index;
    string *recipients, *cc, *tmp_recipients = ({}), *tmp_cc = ({});
    string *to_be_notified = ({});
    mapping inbox_tmp = ([]);
    mapping outbox_tmp = ([]);

    if(clonep(previous_object())) {
        index = strsrch(file_name(previous_object()), "#", -1);

        if(file_name(previous_object())[0..index-1] != OBJ_MAIL_CLIENT[0..<3])
            return "You can only do that through the mail client!";
    }

    else return "You can only do that through the mail client!";

    if(!mail || !sizeof(mail))
        return "message was not sent to mail daemon";

    if(!mail["TO"] || !sizeof(mail["TO"]))
        return "no recipients for the message";

    inbox = ([]);
    outbox = ([]);

    if(!s_editor)
        s_editor = new(OBJ_SECURITY_EDITOR);

    recipients = mail["TO"];

    for(i = 0; i < sizeof(recipients); i++) {
        if(recipients[i][0] == '(' && recipients[i][<1] == ')' && member_array(recipients[i][1..<2], s_editor->list_groups()) != -1)
            tmp_recipients += filter(get_all_members(master()->query_group(recipients[i])), (: $1[0] != '[' :));
        else
            tmp_recipients += ({ lower_case(recipients[i]) });
    }

    recipients = tmp_recipients;

    for(i = 0; i < sizeof(recipients); i++) {
        if(file_exists(get_mail_box_file(recipients[i]))) {
            restore_object(get_mail_box_file(recipients[i]));

            if(member_array(recipients[i], to_be_notified) == -1)
                to_be_notified += ({ recipients[i] });
        }

        inbox_tmp = inbox;
        num = sizeof(inbox_tmp) + 1;
        mail["READ"] = 0;
        inbox_tmp[num] = mail;
        inbox = inbox_tmp;
        save_object(get_mail_box_file(recipients[i]));
    }

    cc = mail["CC"];

    for(i = 0; i < sizeof(cc); i++) {
        if(cc[i][0] == '(' && cc[i][<1] == ')' && member_array(cc[i][1..<2], s_editor->list_groups()) != -1)
            tmp_cc += filter(get_all_members(master()->query_group(cc[i])), (: $1[0] != '[' :));
        else
            tmp_cc += ({ lower_case(cc[i]) });
    }

    cc = tmp_cc;

    for(i = 0; i < sizeof(cc); i++) {
        if(file_exists(get_mail_box_file(cc[i]))) {
            if(member_array(cc[i], to_be_notified) == -1)
                to_be_notified += ({ cc[i] });

            restore_object(get_mail_box_file(cc[i]));
        }

        inbox_tmp = inbox;
        restore_object(get_mail_box_file(cc[i]));
        num = sizeof(inbox_tmp) + 1;
        mail["READ"] = 0;
        inbox_tmp[num] = mail;
        inbox = inbox_tmp;
        save_object(get_mail_box_file(cc[i]));
    }

    for(i = 0; i < sizeof(to_be_notified); i++)
        if(find_player(to_be_notified[i]) && find_player(to_be_notified[i])->query_pref("biff") != "off")
                tell_object(find_player(to_be_notified[i]),

                "\n >>> New mail has arrived from: " + mail["FROM"] +
                "\n >>> Subject: " + mail["SUBJECT"] + "\n\n");

    if(file_exists(get_mail_box_file(owner)))
        restore_object(get_mail_box_file(owner));

    outbox_tmp = outbox;
    num = sizeof(outbox_tmp) + 1;
    outbox_tmp[num] = mail;
    outbox = outbox_tmp;
    curr_in_msg = in_msg;
    curr_out_msg = out_msg;

    save_object(get_mail_box_file(owner));

    if(s_editor)
        s_editor->remove() ;

    return 1;
}

string *get_all_members(string *parent_users) {
    string *users = ({});
    int i;

    for(i = 0; i < sizeof(parent_users); i++) {
        if(parent_users[i][0] == '(' && parent_users[i][<1] == ')' && member_array(parent_users[i][1..<2], s_editor->list_groups()) != -1)
            users += get_all_members(master()->query_group(parent_users[i]));
        else
            users += ({ parent_users[i] });
    }

    return users;
}

varargs mixed delete_message(string owner, int num1, int num2, int flag, int curr_msg) {
    int i, index;
    mapping tmp_box = ([]);
    mapping tmp_box2 = ([]);

    if(clonep(previous_object())) {
        index = strsrch(file_name(previous_object()), "#", -1);

        if(file_name(previous_object())[0..index-1] != OBJ_MAIL_CLIENT[0..<3])
            return "You can only do that through the mail client!";
    }
    else
        return "You can only do that through the mail client!";

    if(file_exists(get_mail_box_file(owner)))
        restore_object(get_mail_box_file(owner));

    if(flag)
        tmp_box = outbox;
    else
        tmp_box = inbox;

    if(num1 < 1)
        num1 = 1;

    if(num2 > sizeof(tmp_box))
        num2 = sizeof(tmp_box);

    if(num2 && (num1 > num2))
        return "invalid range selected";

    if(num2) {
        for(i = 1; i <= sizeof(tmp_box); i++) {
            if(i < num1)
                tmp_box2[i] = tmp_box[i];
            else if(i > num2)
                tmp_box2[i-(num2-num1+1)] = tmp_box[i];
        }
    } else {
        if(num1 == sizeof(tmp_box)) {
            map_delete(tmp_box, num1);
            tmp_box2 = tmp_box;
        } else {
            for(i = 1; i <= sizeof(tmp_box); i++) {
                if(i > num1)
                    tmp_box2[i-1] = tmp_box[i];
                else
                    tmp_box2[i] = tmp_box[i];
            }
        }
    }

    if(flag)
        outbox = tmp_box2;
    else
        inbox = tmp_box2;

    if(flag) {
        if(curr_msg > sizeof(outbox))
            curr_out_msg = sizeof(outbox);
    } else {
        if(curr_msg > sizeof(inbox))
            curr_in_msg = sizeof(inbox);
    }

    save_object(get_mail_box_file(owner));

    return 1;
}

mapping restore(string user) {
    mapping rtn = ([]);

    if(file_exists(get_mail_box_file(user)))
        restore_object(get_mail_box_file(user));

    rtn["inbox"] = inbox;
    rtn["outbox"] = outbox;
    rtn["curr_in_msg"] = curr_in_msg;
    rtn["curr_out_msg"] = curr_out_msg;
    rtn["in_start_index"] = in_start_index;
    rtn["in_end_index"] = in_end_index;
    rtn["out_start_index"] = out_start_index;
    rtn["out_end_index"] = out_end_index;

    return rtn;
}//END restore

void save(string user, mapping in_box, mapping out_box, int *indices) {
    inbox = in_box;
    outbox = out_box;
    curr_in_msg = indices[0];
    curr_out_msg = indices[1];
    in_start_index = indices[2];
    in_end_index = indices[3];
    out_start_index = indices[4];
    out_end_index = indices[5];

    save_object(get_mail_box_file(user));
    return;
}//END save
