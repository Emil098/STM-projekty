#include "console.h"
#include "debugPrintf.h"
#include "debugUart.h"
#include "types.h"
#include "version.h"

#include <string.h>

static char command[MAXCMD + 1] = {0,};
static u8 cmdpos = 0;
bool echo = true;

#define PROMPT   "\n>"

/**
 * @brief Function handler for "say hello" command
 * @param cmd (not used)
 * @retval None  
 */
static void con_cmd_print_hello(char *cmd)
{
    UNUSED(cmd);
    dbg_printf_ln("Hello there");
}

/**
 * @brief Function handler for "print version" command
 * @param cmd (not used)
 * @retval None  
 */
static void con_cmd_print_version(char *cmd)
{
    UNUSED(cmd);
    dbg_printf_ln("firmware version " FIRMWARE_VERSION);
}

/**
 * @brief Function handler for "set echo" command
 * @param cmd string pointer to command's first praram
 * @retval None  
 */
static void con_cmd_set_echo(char *cmd)
{
    if(strncmp(cmd+1, "on", 2) == 0)
    {
        echo = true;
        dbg_printf_ln("echo enabled");
    }
    else if(strncmp(cmd+1, "off", 3) == 0)
    {
        echo = false;
        dbg_printf_ln("echo disabled");
    }
    else
        dbg_printf_ln("Invalid param | echo params: on, off");
}

/**
 * @brief Function for processing command string entered by the user
 * @param new_command string pointer to the command
 * @retval None  
 */
static void con_cmd_process_command(char *new_command)
{
    command_info cmd[] = {
        {"print version", con_cmd_print_version },
        {"say hello"    , con_cmd_print_hello   },
        {"set echo"     , con_cmd_set_echo      }
    };

    int cmd_qt = sizeof(cmd) / sizeof(command_info);
    int i = 0;
    int cmd_text_len;
    
    while(i < cmd_qt)
    {
        cmd_text_len = (int)strlen(cmd[i].cmd_str);
        if (strncmp(new_command, cmd[i].cmd_str, cmd_text_len) == 0)
        {
            cmd[i].handle_func(new_command + cmd_text_len);
            break;
        }
        i++;
    } 
    
    if(i == cmd_qt)
    {
        dbg_printf_ln("Invalid command");
    }
}

/**
 * @brief Main console loop for handling user input and command processing.
 * 
 * This function continuously reads characters from the console input via `dbg_getc()`. 
 * It processes the following special cases:
 * - Carriage return (`0x0D`): Ends input, processes the command if non-empty, and resets the buffer.
 * - Backspace (`0x08`): Deletes the last character in the command buffer if present.
 * For other characters, it stores them in the command buffer and optionally echoes them back if enabled.
 * If the buffer overflows, it notifies the user and resets the buffer.
 * 
 * @retval None
 */
void console_loop(void)
{
    u8 d;
    if (dbg_getc(&d))
    {

        switch (d)
        {

        case 0x0D: // CR
            dbg_printf_ln("");
            command[cmdpos] = '\0';
            if (cmdpos > 0)
                con_cmd_process_command(command);
            else
                dbg_printf_ln("Command is empty");
            
            cmdpos = 0;
            dbg_printf_ln("");
            break;

        case 0x08: // backspace
            if (cmdpos > 0)
            {
                cmdpos--;
                dbg_printf("\x08 \x08");
            }
            break;

        default:
            if (cmdpos < MAXCMD)
            {
                command[cmdpos] = (char)d;
                cmdpos++;
                
                if(echo)
                    dbg_putc(d);
            }
            else
            {
                dbg_printf_ln("\nCommand is already too long");
                cmdpos = 0;
            }
        }
    }
}
