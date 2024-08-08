#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <dirs.h>

// Admin commands
#define CMD_REBOOT           DIR_CMDS_ADM "reboot"
#define CMD_SHUTDOWN         DIR_CMDS_ADM "shutdown"

// Std commands
#define CMD_CHANNEL          DIR_CMDS_STD "channel"
#define CMD_LOOK             DIR_CMDS_STD "look"
#define CMD_QUIT             DIR_CMDS_STD "quit"
#define CMD_SU               DIR_CMDS_STD "su"

#endif // __COMMANDS_H__
