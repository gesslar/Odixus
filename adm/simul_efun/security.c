#include <simul_efun.h>

//security.c

//Tacitus @ LPUniversity
//Grouped on October 22nd, 2005

/**
 * @simul_efun is_member
 * @description Checks if a user is a member of a specified group.
 * @param {string} user - The username to check.
 * @param {string} group - The group to check membership in.
 * @returns {int} - 1 if the user is a member of the group, otherwise 0.
 */
int is_member(string user, string group) {
     if(master()->is_member(user, group)) return 1;
     else return 0;
}

/**
 * @simul_efun adminp
 * @description Checks if a user has admin privileges.
 * @param {mixed} user - The user to check, either as a username string or an
 *                       object. Defaults to the previous object.
 * @returns {int} - 1 if the user has admin privileges, otherwise 0.
 */
int adminp(mixed user) {
     if(!user) user = previous_object();
     if(stringp(user)) {
          if(is_member(user, "admin")) return 1;
     }
     else if(is_member(query_privs(user), "admin")) return 1;
     else return 0;
}

/**
 * @simul_efun devp
 * @description Checks if a user has developer privileges.
 * @param {mixed} user - The user to check, either as a username string or an
 *                       object. Defaults to the previous object.
 * @returns {int} - 1 if the user has developer privileges, otherwise 0.
 */
varargs int devp(mixed user) {
     if(!user) user = previous_object();
     if(stringp(user)) {
          if(is_member(user, "developer")) return 1;
     }
     else if(is_member(query_privs(user), "developer")) return 1;
     else return 0;
}

/**
 * @simul_efun wizardp
 * @description Checks if a user has developer privileges (alias for devp).
 * @param {mixed} user - The user to check, either as a username string or an
 *                       object.
 * @returns {int} - 1 if the user has developer privileges, otherwise 0.
 */
int wizardp(mixed user) { return devp(user); }
