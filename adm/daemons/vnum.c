/**
 * @file /adm/daemons/vnum.c
 * @description Daemon to store room vnums
 *
 * @created 2024-08-18 - Gesslar
 * @last_modified 2024-08-18 - Gesslar
 *
 * @history
 * 2024-08-18 - Gesslar - Created
 */

inherit STD_DAEMON ;

private nomask mapping vnums ;
private nomask mapping reverse_vnums ;
private int next_number ;

void setup() {
    set_persistent(1) ;
}

nomask int get_vnum(mixed room) {
    string fname ;
    int num ;

    if(objectp(room)) fname = base_name(room) ;
    else if(stringp(room)) fname = room ;
    else return 0 ;

    if(!mapp(vnums)) {
        vnums = ([ ]) ;
        reverse_vnums = ([ ]) ;
        next_number = 100 ;
    }

    fname = chop(fname, ".c", -1) ;

    if(of(fname, vnums))
        return vnums[fname] ;

    vnums[fname] = next_number ;
    reverse_vnums[next_number] = fname ;

    num = next_number ++ ;

    save_data() ;

    return num ;
}

nomask string reverse_lookup_vnum( int num ) {
    return reverse_vnums[num] ;
}
