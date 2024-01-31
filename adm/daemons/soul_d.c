/* soul_daemon.c

Parthenon @ LPUniversity
23-JUN-06
LPUni soul daemon

*/

/* Last updated: July 16th, 2006 by Tacitus */

inherit STD_DAEMON ;

/* Defines */

#define SAVE_PATH "/data/daemons/soul_d/soul_d.o"

/* Global Variables */

mapping emotes;

/********************************/
/********** PROTOTYPES **********/
/********************************/

nomask varargs int request_emote(string emote, string who);
nomask varargs void do_emote(string emote, string mods, object target);
nomask void delete_emote(string emote);
nomask void do_save();
nomask void set_emote(string emote, string *msgs);
nomask varargs string build_emote(string msg, string emote, string mods, object target, int plural,
int to_player);
nomask string *query_emotes();
nomask string *query_emote_msgs(string emote);

/* Functions */


void create()
{
    emotes = ([]);
    restore_object(SAVE_PATH);
}

/*********************************************/
/********** EMOTE PERFORM FUNCTIONS **********/
/*********************************************/

nomask varargs int request_emote(string emote, string arg)
{
    object target;
    int i;
    string *tmp_array, *mods_array;
    string mods;

    if(stringp(arg))
    {
        tmp_array = explode(arg, " ");
            target = present(tmp_array[0], environment(this_player()));
    }

    if(target)
    {
        if(member_array(emote+"/t", keys(emotes)) == -1)    return 0;

        if(!present(target, environment(this_player())))
            return notify_fail("That person is not present\n");

        if(!living(target))
            return notify_fail("You can't do that to an inanimate object!\n");

        emote = emote+"/t";
    }

    else    if(member_array(emote, keys(emotes)) == -1)    return 0;

    if( (target && (sizeof(tmp_array) > 1)) || (!target && (sizeof(tmp_array) > 0)) )
    {
        for((target ? i = 1 : i = 0); i < sizeof(tmp_array); i++)
        {
            if(!mods_array) mods_array = ({ tmp_array[i] });
            else mods_array += ({ tmp_array[i] });
        }

        if(sizeof(mods_array) > 0) mods = implode(mods_array, " ");
    }

    else
    {
        if(target) mods = emotes[emote][3];
        else mods = emotes[emote][2];
    }

    if(target) do_emote(emote, mods, target);
    else do_emote(emote, mods);

    return 1;
}

nomask varargs void do_emote(string emote, string mods, object target)
{
    string *msgs;

    msgs = emotes[emote];

    if(emote[<2..<1] == "/t") emote = emote[0..<3];

    if(!target)
    {
        tell_object(this_player(), build_emote(msgs[0], emote, mods, 0, 0, 1) + "\n");
        tell_room(environment(this_player()), build_emote(msgs[1], emote, mods, 0, 1, 0)
             + "\n", this_player());
    }

    else
    {
        tell_object(this_player(), build_emote(msgs[0], emote, mods, target, 0, 1) + "\n");
        tell_room(environment(this_player()), build_emote(msgs[1], emote, mods, target, 1, 0) + "\n", ({ this_player(), target }) );

        if(target != this_player()) tell_object(target, build_emote(msgs[2], emote,
            mods, target, 1, 2) + "\n");
    }//END ELSE

    return;
}

nomask varargs string build_emote(string msg, string emote, string mods, object target, int plural, int to_player)
{
    string *tmp_array = ({});
    string *tmp_array2 = ({});
    int i;

    if(target == this_player())
    {
        if(to_player == 1)
        {
            msg = replace_string(msg, "$PI", "your");
            msg = replace_string(msg, "$I", "You");
        }
        else msg = replace_string(msg, "$I", capitalize(this_player()->query_name()));

        if(to_player == 0) msg = replace_string(msg, "$T", "him/herself");
        else if(to_player == 1) msg = replace_string(msg, "$T", "yourself");
    }

    if(target)
    {
        if(to_player == 1)      msg = replace_string(msg, "$I", "You");
        else msg = replace_string(msg, "$I", capitalize(this_player()->query_name()));

        if(to_player == 1) msg = replace_string(msg, "$PI", "your");
        else msg = replace_string(msg, "$PI", "his/her");

        msg = replace_string(msg, "$SI", "he/she");
        msg = replace_string(msg, "$OI", "him/her");

        if(to_player == 2) msg = replace_string(msg, "$T", "you");
        else msg = replace_string(msg, "$T", capitalize(target->query_name()) );

        if(to_player == 2) msg = replace_string(msg, "$PT", "your");
        else msg = replace_string(msg, "$PT", "his/her");

        msg = replace_string(msg, "$ST", "he/she");
        msg = replace_string(msg, "$OT", "him/her");
        msg = replace_string(msg, "$M", mods);

        if(plural) msg = replace_string(msg, "$V", ((emote[<1] == 's' || emote[<2..<1] == "sh") ? emote+"es" : (emote[<2..<1] == "ch" ? emote+"es" : (emote == "goes" ? "go" : emote+"s"))));
            else msg = replace_string(msg, "$V", emote);
    }//END IF
    else
    {
        if(to_player == 1) msg = replace_string(msg, "$I", "You");
        else if(to_player == 0) msg = replace_string(msg, "$I", capitalize(this_player()->query_name()));

        msg = replace_string(msg, "$PI", "his/her");
            msg = replace_string(msg, "$SI", "he/she");
        msg = replace_string(msg, "$OI", "him/her");
            msg = replace_string(msg, "$M", mods);

        if(plural) msg = replace_string(msg, "$V",
            ((emote[<1] == 's' || emote[<2..<1] == "sh") ? emote+"es"
                : (emote[<2..<1] == "ch" ? emote+"es" :
                (emote == "goes" ? "go" : emote+"s"))));

        else msg = replace_string(msg, "$V", emote);
    }

    tmp_array = explode(msg, " ");

    for(i = 0; i < sizeof(tmp_array); i++)
    {
        if(tmp_array[i] != "") tmp_array2 += ({ tmp_array[i] });
    }

    msg = implode(tmp_array2, " ");

    if(msg[<1] != '.' && msg[<1] != '!' && msg[<1] != '?')    return msg + ".";
    else    return msg;
}

/***********************************************/
/********** EMOTE INTERFACE FUNCTIONS **********/
/***********************************************/

nomask void delete_emote(string emote)
{
    if(base_name(previous_object()) + ".c" != OBJ_SOUL_TOOL) return;
    map_delete(emotes, emote);
    do_save();
}

nomask void set_emote(string emote, string *msgs)
{
    if(base_name(previous_object()) + ".c" != OBJ_SOUL_TOOL) return;

    emotes[emote] = msgs;
    do_save();
}

nomask string *query_emotes()
{
    if(!emotes) restore_object(SAVE_PATH);
    return keys(emotes);
}

nomask string *query_emote_msgs(string emote)
{
    if(!emotes) restore_object(SAVE_PATH);
    return emotes[emote];
}

nomask void reset_emotes()
{
    if(base_name(previous_object()) + ".c" != OBJ_SOUL_TOOL) return;

    if(!emotes) restore_object(SAVE_PATH);
    emotes = ([]);
    save_object(SAVE_PATH);
}

nomask void do_save()
{
    save_object(SAVE_PATH);
}
