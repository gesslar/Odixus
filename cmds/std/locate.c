/* locate.c

 Tacitus @ LPUniversity
 11-AUG-06
 I3 Locate Command

*/

int main(string arg)
{
    mixed *packet;

    if(!stringp(arg)) return notify_fail("SYNTAX: locae <username>\n");

    packet = ({ "locate-req", 5, mud_name(), this_player()->query_name(), 0, 0, arg});
    load_object("/adm/daemons/chmodules/chdmod_i3.c")->send_packet(packet);
    write("Success [locate]: Locate packet sent.\n");
    return 1;
}

string help()
{
    return(" %^BOLD%^SYNTAX:%^RESET%^ locate <username>\n\n"
     "This command allows you to scan the intermud network\n"
     "and locate individuals on other muds that support the\n"
     "locate service. Simply provide the username of the\n"
     "individual you are looking for as an argument to this\n"
     "command and you'll be notified of the results in real\n"
     "time.\n\n"
     " See also: finger, who, tell\n");
}