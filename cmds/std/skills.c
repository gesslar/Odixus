/**
 * @file /cmds/std/skills.c
 * @description Command for players to check their skill progress.
 *
 * @created 2024-07-31 - Gesslar
 * @last_modified 2024-07-31 - Gesslar
 *
 * @history
 * 2024-07-31 - Gesslar - Created
 */

inherit STD_CMD;

private string process_skill_data(object tp, mapping s, string curr, int depth);

void setup() {
    usage_text = "skills\n";
    help_text = "This command will display your current skill levels.\n";
}

mixed main(object tp, string str) {
    mapping skills = tp->query_skills();
    string result;

    result = process_skill_data(tp, skills, "", 0);

    return result;
}

private string process_skill_data(object tp, mapping s, string curr, int depth) {
    string *cles, cle;
    string result = "";

    cles = keys(s);
    cles = sort_array(cles, 1);

    if(strlen(curr))
        curr = curr + ".";

    foreach(cle in cles) {
        int level, progress;
        mapping sub;
        string skill;

        level = tp->query_skill_level(curr+cle);
        progress = tp->query_skill_progress(curr+cle);

        result += sprintf("%3d %*s %s (%d%%)\n", level, depth * 2, "", cle, progress);
        sub = s[cle]["subskills"];
        if(sizeof(sub))
            result += process_skill_data(tp, sub, curr+cle, depth + 1);
    }

    return result;
}
