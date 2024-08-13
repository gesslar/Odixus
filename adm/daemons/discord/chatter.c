/**
 * @file /adm/daemons/modules/chatter.c
 * @description Chatter Discord Bot Example
 *
 * @created 2024-07-08 - Gesslar
 * @last_modified 2024-07-08 - Gesslar
 *
 * @history
 * 2024-07-08 - Gesslar - Created
 */

#include <discord_bot.h>

inherit STD_DISCORD_BOT ;

void setup() {
    start_bot() ;
}

void bot_handle_message_create(mixed data) {
    mapping payload ;

    payload = ([
        "content": "Hello, " + data["author"]["username"] + "!",
        "channel_id": data["channel_id"]
    ]) ;

    call_out_walltime((:
        discord_send_text, EVENT_MESSAGE_CREATE, payload
    :), 2.0) ;
}
