/**
 * @file /std/user/ed.c
 * @description Object to handle interacting with basic or advanced editor.
 *
 * @created 2024-07-20 - Gesslar
 * @last_modified 2024-07-20 - Gesslar
 *
 * @history
 * 2024-07-20 - Gesslar - Created
 */


#include <ed.h>

int ed_write_callback(string file, int flag);
void ed_exit_callback();
void finish_exit_callback();
void abort_edit();

string query_editing();

private nosave string edit_file, temp_file;
private nosave mixed *post_edit;

// When passing a callback to this function, you should have assembled it with
// a string function instead of a function pointer. This is because the
// function pointer may not be valid when the callback is called.
int start_edit(string file, mixed *cb) {
    string text;
    string *lines;
    int sz;

    if(in_edit() || !nullp(edit_file))
        return _error(this_object(), "You are already editing a file.");

    if(nullp(cb))
        return _error(this_object(), "Invalid callback.");

    post_edit = cb;
    temp_file = temp_file(this_object());

    text = "";
    if(!nullp(file)) {
        sz = file_size(file);
        if(sz > -1) {
            catch(cp(file, temp_file));
            if(sz > 0) {
                if(!master()->valid_read(file, this_object(), "ed"))
                    return _error(this_object(), "Permission denied.");
                if(file_size(file) > get_config(__MAX_READ_FILE_SIZE__)) {
                    _error("File is too large to read");
                    abort_edit();
                }
                text = read_file(file);
            }
        }
    }

    if(strlen(text))
        text = append(text, "\n");

    lines = map(explode(text, "\n"), (: "║ " + $1 :));
    text = implode(lines, "\n") + "\n║ ";

    tell(this_object(),
"
    Enter text. When finished, enter '.' on a blank line, or '#' to abort.
  ═══════════════════════════════════════════════════════════════════════════\n");

    tell(this_object(), text);

    input_to("parse_input", 0);
}

void parse_input(string input) {
    if(!input || input == "#")
        return abort_edit();

    if(input == ".")
        return finish_exit_callback();

    write_file(temp_file, input + "\n");

    tell(this_object(), "║ ");

    input_to("parse_input", 0);
}

// When passing a callback to this function, you should have assembled it with
// a string function instead of a function pointer. This is because the
// function pointer may not be valid when the callback is called.
varargs int ed_edit(string file, mixed *cb) {
    string err;

    if(in_edit() || !nullp(edit_file))
        return _error(this_object(), "You are already editing a file.");

    if(!stringp(file))
        return _error(this_object(), "Invalid file name.");

    edit_file = file;
    temp_file = temp_file(this_object());
    post_edit = cb;

    if(file_size(file) > -1) {
        cp(file, temp_file);
    } else {
        touch(temp_file);
    }

    ed(temp_file, "ed_write_callback", "ed_exit_callback");
    return 1;
}

/**
 * When the editor writes to a file, the driver will callback the <write_fn>
 * function twice. The first time, the function is called before the write
 * takes place -- <flag> will be 0. If the function returns TRUE, the write
 * will continue, otherwise it will abort. The second time, the function
 * is called after the write has completed -- <flag> will be non-zero.
 */
int ed_write_callback(string file, int flag) {
    // I can't think of anything I want to do every time the editor writes to
    // a file, so I'm just going to return TRUE.
    return true;
}

void ed_exit_callback(mixed arg...) {
    // Stupid hack to get around the fact that the driver doesn't call the
    // link dead thingie properly.
    call_out_walltime((: finish_exit_callback :), 0.01);
}

void finish_exit_callback() {
    string e;
    int sz;

    defer((: rm, temp_file :));

    if(!interactive(this_object())) {
        catch(abort_edit());
        return;
    }

    // Now let's copy the temp file to the edit file.
    e = catch {
        sz = file_size(temp_file);
        if(sz > 0) {
            if(!nullp(edit_file)) {
                if(!master()->valid_write(edit_file, this_object(), "ed"))
                    throw("Permission denied.");
                cp(temp_file, edit_file);
            }
            if(!nullp(post_edit))
                catch(call_back(post_edit, ED_STATUS_SUCCESS, edit_file, temp_file));
        } else {
            abort_edit();
        }
    };

    if(e)
        catch(abort_edit());

    edit_file = post_edit = temp_file = null;
}

void abort_edit() {
    if(!nullp(temp_file))
        defer((: rm, temp_file :));

    if(!nullp(post_edit))
        catch(call_back(post_edit, ED_STATUS_ABORTED, edit_file, temp_file));

    edit_file = post_edit = temp_file = null;
}

string query_editing() {
    return edit_file;
}
