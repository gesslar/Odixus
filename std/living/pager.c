/**
 * @file /std/user/pager.c
 * @description Pager object for displaying text to the user in a paginated
 *              format.
 *
 * @created 2024-07-23 - Gesslar
 * @last_modified 2024-07-23 - Gesslar
 *
 * @history
 * 2024-07-23 - Gesslar - Created
 */

#include <type.h>
#include <env.h>

// Functions
void continue_page(string input, string *text, mixed *cb, int no_colour, int lines, string display, int num, int curr);

varargs void page(mixed text, mixed *cb, int no_colour) {
    int more_lines, sz;
    string page_display;

    if(nullp(text))
        error("Bad argument 1 to page().");

    if(stringp(text))
        text = explode(text, "\n");

    if(!pointerp(text))
        return 0;

    if(!uniform_array(T_STRING, text))
        return 0;

    more_lines = to_int(query_pref("morelines")) || mud_config("MORELINES");
    page_display = query_pref("page_display") || mud_config("PAGE_DISPLAY");

    if(no_colour == 1)
        no_colour = MSG_PROMPT | NO_COLOUR;
    else
        no_colour = MSG_PROMPT;

    sz = sizeof(text);
    continue_page("", text, cb, no_colour, more_lines, page_display, sz, 0);
}

void continue_page(string input, string *text, mixed *cb, int no_colour, int more_lines, string page_display, int num, int curr) {
    int sz, end;
    string mess;
    string *this_page;

    if(input == "q") {
        if(!nullp(cb))
            call_back(cb);
        return;
    }

    // Calculate the end index for this page
    end = min(({curr + more_lines - 1, sizeof(text) - 1}));

    this_page = text[curr..end];
    mess = implode(this_page, "\n");

    // Update curr for the next iteration
    curr = end + 1;

    if(curr < sizeof(text)) {
        switch(page_display) {
            case "percent" :
                mess += sprintf("\n[%d%% - Press <Return> to continue, q to quit]", percent(curr, num));
                break;
            default:
                mess += sprintf("\n[%d/%d - Press <Return> to continue, q to quit]", curr, num);
                break;
        }

        tell(this_object(), mess, no_colour);
        input_to("continue_page", text, cb, no_colour, more_lines, page_display, num, curr);
    } else {
        tell(this_object(), mess, no_colour);
        if(!nullp(cb))
            call_back(cb);
    }
}
