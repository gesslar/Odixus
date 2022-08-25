/* dest.c

 Tacitus @ LPUniversity
 31-OCT-05
 Standard object minipulation command

*/

//Last edited on July 19th, 2006 by Parthenon

int main(object caller, object room, string str)
{
    string custom, tmp;
    object ob;

    if(!str) str = this_player()->query("cwf");

    if(this_player()->query_env("custom_dest") && wizardp(this_player()))
    custom = this_player()->query_env("custom_dest");

    if(present(str, environment(this_player())))
    {
    ob = present(str, environment(this_player()));
    if(custom)
    {
        tmp = custom;
        tmp = replace_string(tmp, "$O", (ob->query_short()[0] == 'a' ? ob->query_short() : "a " + ob->query_short()));
        tmp = replace_string(tmp, "$N", this_player()->query_cap_name());

        catch(tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player()));
        write("Success [dest]: Destroyed object '" + ob->query_short() + "'.\n");
    }
    else
    {
        write("Success [dest]: Destroyed object '" + ob->query_short() + "'.\n");
        catch(tell_room(environment(this_player()), capitalize(this_player()->query_name()) + " nullifies '" + ob->query_short() + ".\n", ({this_player()})));
    }
    catch(ob->remove());
    if(ob) destruct(ob);
    return 1;
    }

    if(present(str, this_player()))
    {
    ob = present(str, this_player());
    if(custom)
    {
        tmp = custom;
        tmp = replace_string(tmp, "$O", (ob->query_short()[0] == 'a' ? ob->query_short() : "a " + ob->query_short()));
        tmp = replace_string(tmp, "$N", this_player()->query_cap_name());

        catch(tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player()));
        write("Success [dest]: Destroyed object '" + ob->query_short() + "'.\n");
    }
    else
    {
        write("Success [dest]: Destroyed object '" + ob->query_short() + "'.\n");
        catch(tell_room(environment(this_player()), capitalize(this_player()->query_name()) + " nullifies '" + ob->query_short() + ".\n", ({this_player()})));
    }
    catch(ob->remove());;
    if(ob) destruct(ob);
    return 1;          
    }

    if(find_player(str) && adminp(this_player()))
    {
    ob = find_player(str);
    if(custom)
    {
        tmp = custom;
        tmp = replace_string(tmp, "$O", (ob->query_short()[0] == 'a' ? ob->query_short() : "a " + ob->query_short()));
        tmp = replace_string(tmp, "$N", this_player()->query_cap_name());

        catch(tell_room(environment(this_player()), capitalize(tmp) + "\n", this_player()));
        write("Success [dest]: Destroyed object '" + ob->query_short() + "'.\n");
    }
    else
    {
        write("Success [dest]: Destroyed object '" + ob->query_short() + "'.\n");
        catch(tell_room(environment(this_player()), capitalize(this_player()->query_name()) + " nullifies '" + ob->query_short() + ".\n", ({this_player()})));
    }
    tell_object(ob, "Notice [dest]: You have been nullified and hence disconnected from the mud.\n");
    catch(ob->remove());
    if(ob) destruct(ob);
    return 1;
    }

    if(str[<2..<1] != ".c") str += ".c";
    str = resolve_path(this_player()->query("cwd"), str);
    if(find_object(str))
    {
    ob = find_object(str);
    write("Success [dest]: Destructing master object for '" + str + "'.\n");
    this_player()->set("cwf", str);
    catch(ob->remove());
    if(ob) destruct(ob);
    return 1;
    }

    return(notify_fail("Error [dest]: Object '" + str + "' not found.\n"));

}

string help(object caller) {
    return (HIW + " SYNTAX: " + NOR + "dest <object/filename>\n\n" + 
      "This command removes (destorys) an object from memory.\n\n" +
      HIW + "See also: " + NOR + "clone\n"); 
}
