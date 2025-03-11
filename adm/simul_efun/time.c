#include <simul_efun.h>

/**
 * @simul_efun ldate
 * @description Returns a formatted date string.
 * @param {int} [x=time()] - The timestamp to format. Defaults to the current time.
 * @param {int} [brief=0] - If set to 1, returns a brief date format (MM-DD).
 *                          Otherwise, returns a full date format (YYYY-MM-DD).
 * @returns {string} - The formatted date string.
 */
varargs string ldate(int x, int brief) {
    string fmt ;

    if(nullp(x)) x = time() ;

    if(x == 1) {
        brief = 1 ;
        x = time() ;
    }

    if(brief) fmt = "%m-%d" ;
    else fmt = "%Y-%m-%d" ;

    return strftime(fmt, x) ;
}

/**
 * @simul_efun ltime
 * @description Returns a formatted time string.
 * @param {int} [x=time()] - The timestamp to format. Defaults to the current time.
 * @param {int} [brief=0] - If set to 1, returns a brief time format (HH:MM).
 *                          Otherwise, returns a full time format (HH:MM:SS).
 * @returns {string} - The formatted time string.
 */
varargs string ltime(int x, int brief) {
    string fmt ;

    if(nullp(x)) x = time() ;

    if(x == 1) {
        brief = 1 ;
        x = time() ;
    }

    if(brief) fmt = "%H:%M" ;
    else fmt = "%H:%M:%S" ;

    return strftime(fmt, x) ;
}

/**
 * @simul_efun ldatetime
 * @description Returns a formatted date and time string.
 * @param {int} [x=time()] - The timestamp to format. Defaults to the current
 *                           time.
 * @param {int} [brief=0] - If set to 1, returns a brief date and time format
 *                          (MM-DD HH:MM). Otherwise, returns a full date and
 *                          time format (YYYY-MM-DD HH:MM:SS).
 */
varargs string ldatetime(int x, int brief) {
    return ldate(x, brief) + " " + ltime(x, brief) ;
}

/**
 * @simul_efun time_ms
 * @description Converts a time value from nanoseconds to milliseconds.
 *              This function takes a time value in nanoseconds and converts it
 *              to milliseconds by dividing the input by 1,000,000.
 * @param {int} nanoseconds - The time value in nanoseconds. Defaults to the
 *                            current time in nanoseconds if not provided.
 * @returns {int} - The time value converted to milliseconds.
 */
varargs int time_ms(int nanoseconds) {
    nanoseconds = nanoseconds || time_ns() ;
    return nanoseconds / 1_000_000 ;
}

/**
 * @simul_efun time_frac
 * @description Converts a time value from nanoseconds to a fractional value in seconds.
 *              This function takes a time value in nanoseconds and converts it to
 *              a fractional value representing seconds by dividing the input by 1,000,000,000.
 * @param {int} nanoseconds - The time value in nanoseconds. Defaults to the current
 *                            time in nanoseconds if not provided.
 * @returns {float} - The time value converted to a fractional value in seconds.
 */
varargs float time_frac(int nanoseconds) {
    nanoseconds = nanoseconds || time_ns() ;
    return nanoseconds / 1_000_000_000.0 ;
}
