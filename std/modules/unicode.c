/**
 * @file /std/modules/unicode.c
 * @description Module for handling unicode characters.
 *
 * Portions of this were delightfully and unabashedly stolen from the Lima base
 * library.
 *
 * @created 2024/07/18 - Gesslar
 * @last_modified 2024/07/18 - Gesslar
 *
 * @history
 * 2024/07/18 - Gesslar - Created
 */

/**
Original Lima header:
/*
** M_WIDGETS (Tsath 2019-2020)
**
** This module returns nicely formatted ANSI/xterm type widgets that can be
** reused all over the MUD. The string return are not sprintf() compatible, so
** you need to write() or out() them.
**
*/

private nomask nosave mapping _c = ([
    "green" : "\e0036\e",
    "blue"  : "\e0027\e",
    "purple": "\e0127\e",
    "yellow": "\e0185\e",
    "gold"  : "\e0220\e",
    "black" : "\e0238\e",
    "white" : "\e0015\e",
]) ;

private nomask nosave string *slider_colours = ({
    "\e0009\e", // red
    "\e0202\e", // dark orange
    "\e0208\e", // orange
    "\e0214\e", // light orange
    "\e0220\e", // gold
    "\e0226\e", // yellow
    "\e0190\e", // light yellow
    "\e0154\e", // light green
    "\e0118\e", // green
    "\e0047\e", // bright green
});

/**
 * @local_function uses_unicode
 * @description Returns true if the this_body() supports unicode
 * @returns {int} - 1 if the body supports unicode, 0 otherwise
 */
int supports_unicode() {
    object body = this_body() ;
    int unicode ;

    if(!body)
        return 0 ;

    if(body->has_screenreader())
        return 0 ;

    return body->supports_unicode() ;
}

//: FUNCTION default_user_width
// Returns the user screen width -2 chars.
int default_user_width() {
    object body = this_body() ;

    if(body)
        if(body->query_environ("WORD_WRAP"))
            return body->query_environ("WORD_WRAP") - 2 ;

    return 78 ;
}

//: FUNCTION use_colour
// Returns the colour in the colour array, cols, to use given the position
// out of the width (typically screen width).
string use_colour(string *cols, int position, int width) {
   int col_index = floor(sizeof(cols) * ((0.0 + position) / (width || 1)));

   col_index = range(col_index, 0, sizeof(cols) - 1);
   return cols[col_index] ;
}

//: FUNCTION gradient_string
// Given a string, spread the colour array equally over the string length.
// Example::
//   gradient_string("Hello world",({"118","119","120","121","122","123"}))
string gradient_string(string str, string *cols) {
    int i = 0;
    string *exstr = explode(str, "");
    while(i < strlen(str)) {
        exstr[i] = use_colour(cols, i, strlen(str)) + exstr[i];
        i++;
    }
    return implode(exstr, "") + "\eres\e" ;
}

//: FUNCTION on_off_widget
// [On ] (green) or [Off] (red) depending on int on.
// "On " or "Off" for simplified view.
string on_off_widget(int on) {
    object body = this_body() ;

    if(body->has_screenreader())
        return on ? "On " : "Off ";
    if(on)
        return supports_unicode() ? "[ \e0036\e✓\eres\e ]" : sprintf("[\e0036\eOn\eres\e ]");
    else
        return supports_unicode() ? "[ \e0243\e✕\eres\e ]" : sprintf("[\e0243\eOff\eres\e]");
}

//: FUNCTION simple_divider
// Prints a simple divider either using unicode or not, depending on user settings.
varargs string simple_divider(int width) {
    object body = this_body() ;
    string barchar = supports_unicode() ? "─" : "-";

    if(body->has_screenreader())
        return "";

    if(!width)
        width = default_user_width();

    return repeat_string(barchar, width) ;
}

//: FUNCTION simple_divider
// Prints a simple double divider either using unicode or not, depending on user settings.
varargs string simple_double_divider(int width) {
    object body = this_body() ;
    string barchar = supports_unicode() ? "═" : "=";

    if(body->has_screenreader())
        return "";

    if(!width)
        width = default_user_width();

    return repeat_string(barchar, width) ;
}

//: FUNCTION green_bar
// A bar that is green with white dots when spent
string green_bar(int value, int max, int width) {
    object body = this_body() ;
    int green, white;
    string barchar = supports_unicode() ? "▅" : "=";
    string nobarchar = supports_unicode() ? "▅" : ".";

    if(body->has_screenreader())
        return value + "/" + max;

    if(value > max)
        value = max;

    green = (value * 1.00 / max) * (width)-1;
    white = width - 2 - green;

    return sprintf(
        "[" + (value >= max ? _c["green"] : _c["blue"]) + "%s\eres\e"+_c["black"]+"`%s\eres\e]",
            repeat_string(barchar, green), repeat_string(nobarchar, white));
}

//: FUNCTION critical_bar
// A bar that change colour the lower it gets
string critical_bar(int value, int max, int width) {
    object body = this_body() ;
    int green, white;
    float p;
    string barchar = supports_unicode() ? "▅" : "=";
    string nobarchar = supports_unicode() ? "▅" : ".";
    string bar_colour = "\e0036\e";

    if(body->has_screenreader())
        return value + "/" + max;

    p = value / (max * 1.0);

    if(p < 0.10)
        bar_colour = _c["purple"] ;
    else if(p < 0.20)
        bar_colour = _c["yellow"] ;
    else if(p < 0.50)
      bar_colour = _c["gold"] ;

    if(value > max)
        value = max;

    green = (value * 1.00 / max) * (width)-1;
    if(green < 0)
        green = 0;
    white = width - 1 - green;

    return sprintf(
        "[" + bar_colour + "%s\eres\e"+_c["black"]+"%s\eres\e]",
            repeat_string(barchar, green), repeat_string(nobarchar, white));
}

//: FUNCTION reverse_critical_bar
// A bar that change colour the lower it gets
string reverse_critical_bar(int value, int max, int width) {
    object body = this_body() ;
    int green, white;
    float p;
    string barchar = supports_unicode() ? "▅" : "=";
    string nobarchar = supports_unicode() ? "▅" : ".";
    string bar_colour = "\e0036\e";

    if(!max)
        return "";

    if(body->has_screenreader())
        return value + "/" + max;

    p = value / (max * 1.0);

    if(p < 0.30)
        bar_colour = _c["green"] ;
    if(p < 0.60)
        bar_colour = _c["gold"] ;
    else if(p < 0.80)
        bar_colour = _c["yellow"] ;

    if(value > max)
        value = max;

    green = (value * 1.00 / max) * (width)-1;
    if(green < 0)
        green = 0;
    white = width - 1 - green;

    return sprintf("[" + bar_colour + "%s\eres\e"+_c["black"]+"%s\eres\e]",
        repeat_string(barchar, green), repeat_string(nobarchar, white));
}

//: FUNCTION slider_red_green
// A slider which is red below the middle and green above, and marks the
// value with a X. 0 is the middle and max is minus on red axis and + on green axis.
string slider_red_green(int value, int max, int width) {
    object body = this_body() ;
    string return_string;
    int marker;
    string x_char;
    string line_char;

    if(body->has_screenreader())
        return value + "/" + max;

    if(supports_unicode()) {
        x_char = "●";
        line_char = "▬";
    } else {
        x_char = "X";
        line_char = "-";
    }

    width = width - 2; // [, X and ]

    marker = width * ((value + (max / 2.0)) / (max * 1.0));

    return_string = repeat_string(line_char, marker) + x_char + repeat_string(line_char, width - marker);
    return_string = return_string[0..(width / 2)] + return_string[(width / 2 + 1)..];
    return_string = gradient_string(return_string, slider_colours);
    return_string = replace_string(return_string, x_char, _c["white"]+ x_char + "\eres\e");
    return "[" + return_string + "\eres\e] ";
}

//: FUNCTION slider_colours_sum
// A slider with multiple colours and cumulative ranges and a marker.
// The colours mapping should be on the format:
//   ([20:"040",50:"041",100:"042"])
// where each number is bigger and strings are ANSI colours.
string slider_colours_sum(int value, mapping colours, int width) {
    object body = this_body() ;
    int marker;
    int max = sort_array(keys(colours), -1)[0];
    string return_string;
    int colour_add = 0;
    int next_pos = 0;
    string x_char;
    string line_char;
    string colour_after_marker; // Save this colour to make things easier later.

    if(body->has_screenreader())
        return value + "/" + max;

    if(supports_unicode()) {
        x_char = "●";
        line_char = "▬";
    } else {
        x_char = "X";
        line_char = "-";
    }
    width = width - 3; // [ and ]
    marker = width * (1.0 * value / max);

    if(marker == 0)
        return_string = x_char + repeat_string(line_char, width - 1);
    else if(marker == (width - 1))
        return_string = repeat_string(line_char, width) + x_char;
    else
        return_string = repeat_string(line_char, marker) + x_char + repeat_string(line_char, width - marker);

    foreach(int val in sort_array(keys(colours), 1)) {
        string col = colours[val];
        if(!next_pos)
            return_string = col + return_string;
        if(next_pos)
            return_string = return_string[0..(next_pos + colour_add)] + col +
                            return_string[((next_pos + colour_add) + 1)..];
        colour_add += strlen(col) ; // TODO: Check this is correct
        next_pos = width * (1.0 * val / max);
        if(next_pos > marker && !colour_after_marker)
            colour_after_marker = col;
    }

    return_string = replace_string(return_string, x_char, _c["white"] + x_char + colour_after_marker);

    return "[" + return_string + "\eres\e] ";
}
