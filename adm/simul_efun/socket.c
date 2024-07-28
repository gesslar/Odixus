#include <simul_efun.h>

/* socket.c

 Tacitus @ LPUniversity
 05-MAY-06
 Socket related simul-efuns

*/

/**
 * @simul_efun dump_socket_status
 * @description Returns a formatted string displaying the status of all sockets.
 * @returns {string} - The formatted socket status information.
 */
string dump_socket_status() {
    string ret =
"Fd    State      Mode       Local Address          Remote Address            PROGRAM\n"
"--  ---------  --------  ---------------------  --------------------- ---------------------\n";

    foreach (mixed item in socket_status()) {
        ret += sprintf("%2d  %|9s  %|8s  %-21s  %-21s %-21O\n", item[0], item[1], item[2], item[3], item[4], objectp(item[5]) ? item[5] : 0 );
    }

    return ret;
}
