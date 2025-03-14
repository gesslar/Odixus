// /cmds/std/gmcp.c
// Basic command for ineracting with the GMCP system
//
// Created:     2024/02/23: Gesslar
// Last Change: 2024/02/23: Gesslar
//
// 2024/02/23: Gesslar - Created

#include <gmcp_defines.h>

inherit STD_CMD;

mixed gmcp_status(object tp);
void gmcp_gui_result(mapping response, object tp);

void setup() {
    usage_text =
"gmcp - Displays your current GMCP status.\n"
"gmcp <on/off> - Enables or disables GMCP.\n"
"gmcp gui - Requests the Mudlet GUI install from the server.\n";

    help_text =
"Displays your current GMCP status or enables/disables GMCP. GMCP is a "
"protocol that allows the server to send information to the client. This "
"information can be used to enhance the user interface and provide additional "
"information to the player. GMCP is only available if the client supports it "
"and it is enabled in the client and the server.\n"
"\n"
"You can also use the 'gui' option to request the Mudlet GUI information from "
"the server. This will automatically install the latest version of the GUI "
"for the "+mud_name()+" client.\n";
}

mixed main(object tp, string arg) {
    if(!get_config(__RC_ENABLE_GMCP__))
        return "GMCP is not enabled on the server.";

    if(!arg)
        return gmcp_status(tp);

    if(arg == "on") {
        if(tp->gmcp_enabled()) {
            return "GMCP is already enabled.";
        }
        if(!has_gmcp(tp)) {
            return "GMCP is not available. Ensure you are using a GMCP "
                "capable client and that GMCP is enabled in the client.";
        }
        tp->set_env("gmcp", "on");
        GMCP_D->init_gmcp(tp);
        return "GMCP is now enabled.";
    } else if(arg == "off") {
        if(!tp->gmcp_enabled()) {
            return "GMCP is already disabled.";
        }
        if(!has_gmcp(tp)) {
            return "GMCP is not available. Ensure you are using a GMCP "
                "capable client and that GMCP is enabled in the client.";
        }
        tp->set_env("gmcp", "off");
        return "GMCP is now disabled.";
    } else if(arg == "gui") {
        if(!tp->gmcp_enabled()) {
            return "GMCP is not enabled.";
        }
        if(!has_gmcp(tp)) {
            return "GMCP is not available. Ensure you are using a GMCP "
                "capable client and that GMCP is enabled in the client.";
        }

        HTTPC_D->fetch(
            assemble_call_back((: gmcp_gui_result :), tp),
            "GET",
            "https://api.github.com/repos/gesslar/gLPUi/releases/latest",
            ([ "Accept" : "application/vnd.github.v3+json" ]),
        );

        return "Initiating "+mud_name()+" GUI request...";
    }

    return "Syntax: gmcp OR gmcp [on|off]";
}

mixed gmcp_status(object tp) {
    if(tp->gmcp_enabled()) {
        return "GMCP is currently enabled.";
    }
    if(!has_gmcp(tp)) {
        return "GMCP is not available. Ensure you are using a GMCP "
            "capable client and that GMCP is enabled in the client.";
    }
    return "GMCP is currently disabled.";
}

void gmcp_gui_result(mapping response, object tp) {
    string url;
    string version;
    mapping data;

    if(!response) {
        tell(tp, "Error fetching version information.\n");
        return;
    }

    if(response["status"]["code"] != 200) {
        tell(tp, "Error fetching version information.\n");
        return;
    }

    data = json_decode(response["body"]);
    version = data["tag_name"];
    foreach(mapping value in data["assets"]) {
        if(pcre_match(value["name"], "\\.mpackage")) {
            url = value["browser_download_url"];
            break;
        }
    }

    if(!url) {
        tell(tp, "Error determining download URL.\n");
        return;
    }

    GMCP_D->send_gmcp(tp,
        GMCP_PKG_CLIENT_GUI_INSTALL,
        ([
            GMCP_LBL_GMCP_CLIENT_GUI_URL : url,
            GMCP_LBL_GMCP_CLIENT_GUI_VERSION : version,
        ])
    );
}

string help(object who) {
    return
"Syntax: gmcp\n"
"        gmcp <on/off>\n"
"        gmcp gui\n\n"
"Displays your current GMCP status or enables/disables GMCP. GMCP is a "
"protocol that allows the server to send information to the client. This "
"information can be used to enhance the user interface and provide additional "
"information to the player. GMCP is only available if the client supports it "
"and it is enabled in the client and the server.\n"
"\n"
"You can also use the 'gui' option to request the GUI information from the "
"server. This will automatically install the latest version of the GUI for "
"the "+mud_name()+" client.\n";
}
