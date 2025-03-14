/**
 * @file /adm/daemons/websocket_echo.c
 * @description Test websocket with echo.websocket.org
 *
 * @created 2024-07-05 - Gesslar
 * @last_modified 2024-07-05 - Gesslar
 *
 * @history
 * 2024-07-05 - Gesslar - Created
 */

#include <daemons.h>
#include <websocket.h>
#include <socket.h>
#include <socket_err.h>

inherit STD_WS_CLIENT;

private nosave string host = "localhost";
private nosave string path = null;
private nosave string server_name = "WebSocket Echo Server";

protected void periodic_message();
protected void send_outgoing_message(string message);
protected void close_down_session();

private nosave mapping state = ([ ]);

void setup() {
    set_log_level(4);
    set_log_prefix("(WSS ECHO)");
    // call_out("startup", 1);
}

// Start the Connection
varargs void startup() {
    websocket_connect("wss://echo.websocket.org");
}

void websocket_handle_connected() {
    int closing_time = 20+random(20);

    state = ([]);

    _log(1, "Scheduling periodic messaging");
    state["messaging"] = call_out_walltime((: periodic_message :), 5);

    _log(1, "Scheduling close down in %d seconds", closing_time);
    state["closing"] = call_out_walltime((: close_down_session :), closing_time);
}

void websocket_handle_shutdown() {
    int messaging, closing;

    if(!server) {
        _log(2, "Not connected to any server");
        return;
    }

    messaging = state["messaging"];
    if(!nullp(messaging)) {
        if(find_call_out(messaging) != -1) {
            _log(1, "Stopping automatic messaging");
            remove_call_out(messaging);
        }
    }

    closing = state["closing"];
    if(!nullp(closing)) {
        if(find_call_out(closing) != -1) {
            _log(1, "Stopping automatic close down");
            remove_call_out(closing);
        }
    }

    state = null;
}

// Function to parse text frames
void websocket_handle_text_frame(string payload) {
    mixed message;

    if(!state)
        return;

    if(!state["messaging"])
        return;

    if(!state["last_message"])
        return;

    if(nullp(payload)) {
        _log(2, "Received null payload");
        return;
    }

    message = parse_body(payload, "auto");

    if(!stringp(message)) {
        _log(2, "Received non-string payload: %O", message);
        return;
    }

    _log(1, "Received message: %O", message);

    if(message == state["last_message"]) {
        _log(1, "==> Messages match! 🍻");
    } else {
        _log(1, "==> Messages do not match! 😭");
    }
}

void periodic_message() {
    int messaging;
    string message;

    if(!server || !state)
        return;

    messaging = state["messaging"];
    if(find_call_out(messaging) != -1) {
        _log(1, "Canceling existing messaging schedule");
        remove_call_out(messaging);
    }

    message = "Hello, world - " + base64_encode(""+time());

    _log(2, "Sending periodic message");
    send_outgoing_message(message);

    state["last_message"] = message;

    _log(2, "Scheduling next message");
    state["messaging"] = call_out_walltime((: periodic_message :), 10+random(20));
}

void send_outgoing_message(string message) {
    _log(1, "Sending message: %O", message);

    if(!websocket_message(WS_TEXT_FRAME, message))
        _log(1, "Failed to send message");
}

void close_down_session() {
    _log(1, "Automatically closing down session");

    if(!websocket_close(WS_CLOSE_NORMAL, "Self-initiated close down"))
        _log(1, "Failed to send close frame");
}
