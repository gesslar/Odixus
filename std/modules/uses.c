/**
 * @file /std/modules/uses.c
 * @description This module is inherited in order to provide consumabling.
 *
 * @created 2024-08-06 - Gesslar
 * @last_modified 2024-08-06 - Gesslar
 *
 * @history
 * 2024-08-06 - Gesslar - Created
 */

protected nomask int _max_uses = null ;
protected nomask int _uses = null ;
protected nomask string _use_status_message = null ;

int set_uses(int uses) {
    _uses = uses;

    if(nullp(_max_uses)) {
        _max_uses = uses;
    }

    return _uses;
}

int query_uses() {
    return _uses;
}

int query_max_uses() {
    return _max_uses;
}

mixed adjust_uses(int uses) {
    if(nullp(_uses) || nullp(_max_uses))
        return null ;

    if(_uses + uses < 0)
        return null ;

    if(_uses + uses > _max_uses)
        return null ;

    _uses += uses;

    return _uses;
}

void set_use_status_message(string message) {
    _use_status_message = message;
}

string query_use_status_message() {
    return _use_status_message;
}

void reset_uses() {
    _uses = _max_uses;
}
