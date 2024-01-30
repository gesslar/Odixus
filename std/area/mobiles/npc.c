/* npc.c

 Tacitus @ LPUniversity
 28-JUNE-06
 Non-player character

*/

inherit MOBILE;

void create() {
    path = ({ "/cmds/std/" });
    set("prevent_get", 1);
    if(!query("cwd")) set("cwd", "/doc");
    if(!mapp(query("env_settings"))) set("env_settings", (["colour" : "enabled"]));
    if(!query_env("news_client")) set_env("news_client", "/obj/mudlib/newsclients/std_newsclient.c");
    if(!query_env("auto_tune")) set_env("auto_tune", "localNet");
    setup();
    enable_commands();
    add_action("commandHook", "", 1);
}

void setup() {
    set_name("Generic NPC");
    set_living_name(lower_case(query_name()));
    set_ids(({query_name(), "npc", "generic npc", "NPC" }));
    if(clonep(this_object())) set_heart_beat(1);
    set_long(capitalize(query_name()) + " is a generic NPC.");
    set_short(query_name());
}

int commandHook(string arg) {
    return ::commandHook(arg);
}

void restore_user() {}
void save_user() {}
void heart_beat() {}

int force_me(string cmd)
{
    command(cmd);
}
