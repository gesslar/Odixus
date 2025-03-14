/**
 * @file /cmds/wiz/summon.c
 * @description Command to summon a player to your location.
 *
 * @created 2024-08-17 - Gesslar
 * @last_modified 2024-08-17 - Gesslar
 *
 * @history
 * 2024-08-17 - Gesslar - Created
 */

inherit STD_CMD;

void setup() {
    usage_text = "summon <user>";
    help_text =
"This command allows you to move a user to your environment. You cannot "
"summon admins, and if you are a developer you cannot summon other "
"developers.\n\n"
"Use this command with care, as it can be disruptive to the game experience.";
}

mixed main(object tp, string arg) {
    object target, env, tenv;
    string name, tname;

    if(!arg || arg == "")
        return _usage(tp);

    if(!target = find_player(arg))
        return _error("Cannot find a player named %s", arg);

    env = environment(tp);
    if(!env)
        return _error("You have no environment to summon %s to!", tname);

    tenv = environment(target);
    if(!tenv)
        return _error("%s has no environment.", tname);

    name = tp->query_name();
    tname = target->query_name();

    if(env == tenv)
        return _info("%s is already here.", tname);

    if(adminp(target) || (devp(target) && !adminp(tp)))
        return _error("You may not summon %s.", tname);

    if(target->move(env))
        return _error("Could not move %s to you.", tname);

    tell(target, "You have been summoned to " + name + ".\n");

    return _ok(tp, "You have summoned %s to you.", tname);
}
