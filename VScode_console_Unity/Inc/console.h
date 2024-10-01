#ifndef INC_CONSOLE_H_
#define INC_CONSOLE_H_

#define MAXCMD   70

typedef void (*cmd_func)(char *cmd);

typedef struct
{
    char cmd_str[MAXCMD];
    cmd_func handle_func;
}command_info;

void console_loop(void);

#endif