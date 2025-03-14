/**
 * @file /cmds/wiz/touch.c
 * @description This command allows a wizard to create a new empty file.
 *
 * @created 2024-08-10 - Gesslar
 * @last_modified 2024-08-10 - Gesslar
 *
 * @history
 * 2024-08-10 - Gesslar - Created
 */

inherit STD_CMD;

void setup() {
    usage_text = "touch [-f] <file>";
    help_text =
"Creates a new empty file at the specified path. If the path does not exist, "
"the function will fail to create the file unless the -f flag is used to "
"force the creation of the necessary directories.";
}

mixed main(object tp, string str) {
    string file;
    int ret;
    int force;
    string *parts;

    if(!str || str == "")
        return _info("Syntax: touch [-f] <file>");

    if(sscanf(str, "-f %s", str))
        force = 1;

    file = resolve_path(tp->query_env("cwd"), str);
    parts = dir_file(file);

    if(!directory_exists(parts[0]))
        if(!force)
            return _error("No such directory: %s", parts[0]);

    ret = touch(file);
    if(!ret)
        return _error("Failed to touch file: %s", file);

    return _ok("Touched file: %s", file);
}
