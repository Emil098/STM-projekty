#include "unity.h"
#include "unity_fixture.h"
#include "cmock.h"

#include "mock_debugPrintf.h"
#include "mock_debugUart.h"

#include "debugPrintf.h"
#include "debugUart.h"
#include "console.h"
#include "version.h"

#include <string.h>


static void dbg_getc_cmd_pass(char *cmd, int len,bool endl, bool echo_on)
{
    char empty_ptr = 0;
    char nl = '\r';
    int i = 0;

    for(i=0;i<len;i++)
    {
        dbg_getc_ExpectAndReturn((uint8_t *)&empty_ptr, true);
        dbg_getc_ReturnThruPtr_d((uint8_t *)cmd);
        if(echo_on)
            dbg_putc_ExpectAndReturn((uint8_t)*cmd,true);
        
        console_loop();
        cmd++;
    }

    if(endl)
    {
        dbg_getc_ExpectAndReturn((uint8_t *)&empty_ptr, true);
        dbg_getc_ReturnThruPtr_d((uint8_t *)&nl);
        console_loop();
    }
}

TEST_GROUP(test_consoleToUT);

TEST_SETUP(test_consoleToUT)
{
    char cmd[] = "set echo off";

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("echo disabled");
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(cmd, (int) strlen(cmd), true, true);
}

TEST_TEAR_DOWN(test_consoleToUT)
{

}

TEST(test_consoleToUT, test_print_hello)
{
    char cmd[] = "say hello";

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("Hello there");
    dbg_printf_ln_Expect("");
    

    dbg_getc_cmd_pass(cmd, (int) strlen(cmd), true, false);
}

TEST(test_consoleToUT, test_print_version)
{
    char cmd[] = "print version";
    char version_str[22];

    sprintf(version_str,"firmware version %s",FIRMWARE_VERSION);

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect(version_str);
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(cmd, (int) strlen(cmd), true, false);
}

TEST(test_consoleToUT, test_invalid_cmd)
{
    char cmd[] = "invalid";

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("Invalid command");
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(cmd, (int) strlen(cmd), true, false);
}

TEST(test_consoleToUT, test_echo)
{
    char cmd[] = "set echo on";
    char cmd2[] = "set echo off";

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("echo enabled");
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(cmd, (int) strlen(cmd), true, false);

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("echo disabled");
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(cmd2, (int) strlen(cmd2), true, true);
}

TEST(test_consoleToUT, test_echo_invalid_param)
{
    char cmd[] = "set echo XYZ";

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("Invalid param | echo params: on, off");
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(cmd, (int) strlen(cmd), true, false);
}

TEST(test_consoleToUT, test_too_long_cmd)
{
    char cmd[MAXCMD + 1];
    memset(cmd, '0', MAXCMD + 1);

    dbg_printf_ln_Expect("\nCommand is already too long");
    dbg_getc_cmd_pass(cmd, MAXCMD + 1, false, false);
}

TEST(test_consoleToUT, test_empty_cmd)
{
    char cmd = '\r';

    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("Command is empty");
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(&cmd, 1, false, false);
}

TEST(test_consoleToUT, test_backspace)
{
    char cmd[] = "sag\x08y hello";

    dbg_printf_Expect("\x08 \x08");
    dbg_printf_ln_Expect("");
    dbg_printf_ln_Expect("Hello there");
    dbg_printf_ln_Expect("");
    
    dbg_getc_cmd_pass(cmd,(int) strlen(cmd), true, false);
}
