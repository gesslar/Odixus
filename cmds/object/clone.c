/* clone.c

 Tacitus @ LPUniversity
 31-OCT-05
 Standard object minipulation command

*/

//Last edited on July 19th, 2006 by Parthenon

inherit STD_CMD;

mixed main(object tp, string str) {
  object ob, dest, env;
  string err, custom, tmp, short, file;
  int result;

  if(!str)
      str = tp->query_env("cwf");

  if(!str)
    return "SYNTAX: clone <filename>";

  env = environment(tp);

  if(ob = get_object(str)) {
    if(virtualp(ob))
      str = ob->query_virtual_master();
    else
      str = base_name(ob);

    _info(tp, "Making a copy of %O.", ob);

    ob = null;
  }

  str = resolve_path(tp->query_env("cwd"), str);

  err = catch(ob = new(str));

  if(stringp(err))
    return _error("An error was encountered when cloning the object:\n%s", err);

  if(!ob)
    return _error("Unable to clone the object.");

  short = get_short(ob);
  file = file_name(ob);
  dest = tp;

  result = ob->move(dest);
  if(result == MOVE_OK) {
    if(tp->query_env("custom_clone") && wizardp(tp))
      custom = tp->query_env("custom_clone");
      if(custom) {
        tmp = custom;
        tmp = replace_string(tmp, "$O", short);
        tmp = replace_string(tmp, "$N", tp->query_name());
        tell_them(capitalize(tmp) + "\n");
        _ok(file + "' cloned to " + get_short(dest) + " (" +file_name(dest)+ ").\n");
      } else {
        _ok(file + "' cloned to " + get_short(dest) + " (" +file_name(dest)+ ").\n");
        tell_them(capitalize(tp->query_name()) + " creates " + short + ".\n");
      }

      tp->set_env("cwf", str);

      return 1;
    } else {
      if(result == MOVE_TOO_HEAVY) {
        ob->move(env);
        return _ok("%s was moved to the room.", short);
      }

      if(result == MOVE_NO_DEST)
          return _error("The object has no destination.");

      if(result == MOVE_NOT_ALLOWED)
          return _error("You are not allowed to carry the object.");

      if(result == MOVE_DESTRUCTED)
          return _error("The object has been destructed.");
    }

  if(tp->query_env("custom_clone") && wizardp(tp))
    custom = tp->query_env("custom_clone");

  if(custom) {
    tmp = custom;
    tmp = replace_string(tmp, "$O", short);
    tmp = replace_string(tmp, "$N", tp->query_name());
    tell_them(capitalize(tmp) + "\n");
    _ok(file + "' cloned to " + get_short(dest) + " (" +file_name(dest)+ ").\n");
  } else {
    _ok(file + "' cloned to " + get_short(dest) + " (" +file_name(dest)+ ").\n");
    tell_them(tp->query_name() + " creates " + short + ".\n");
  }

  tp->set_env("cwf", str);
  return 1;
}

string help(object tp) {
  return (" SYNTAX: clone <file>\n\n" +
  "This command produces a clone of a file.\n\n" +
  "See also: dest");
}
