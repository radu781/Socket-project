#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <utmp.h>
#include "user_info.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/memory.h"

#define _COMMAND_COUNT 6

const char supportedCommands[_COMMAND_COUNT][17] = {
    "login", "get-logged-users", "get-proc-info", "logout", "quit", "help"};

typedef struct Command
{
    char *body;
    char **args;
    unsigned argCount;
};

bool _findUser(const char *username)
{
    FILE *_configFile = fopen(".config", "r");
    char buff[_USERLEN];

    while (fscanf(_configFile, "%s", buff) != EOF)
        if (strcmp(buff, username) == 0)
        {
            fclose(_configFile);
            return true;
        }

    fclose(_configFile);
    return false;
}

char *_login(const struct Command *cmd)
{
    if (cmd->argCount == 0)
        return "Usage: login <user>";

    if (_loggedIn)
    {
        char *buff = (char *)allocatePtr(sizeof(char), 47 + _USERLEN);
        snprintf(buff, 47 + _USERLEN, "Already logged in as %s, try \"logout\" first", _username);
        _haveAllocated = true;
        return buff;
    }
    if (_findUser(cmd->args[0]))
    {
        strcpy(_username, cmd->args[0]);
        logComm(stdout, "Logged in as %s", _username);
        _loggedIn = true;
        return "";
    }

    return "User not found";
}

char *_getUsers(const struct Command *cmd)
{
    if (!_loggedIn)
        return "This command is available only to logged users, try \"login <user>\"";

    char *out = (char *)allocatePtr(sizeof(char), 128 * 12);
    _haveAllocated = true;

    for (;;)
    {
        struct utmp *ut = getutent();
        if (ut == NULL)
            break;

        char *buff = (char *)allocatePtr(sizeof(char), 128);
        snprintf(buff, 128, "user: %s\nhost: %s\nseconds: %d\nmicrosec: %d\n\n", ut->ut_user, ut->ut_host, ut->ut_tv.tv_sec, ut->ut_tv.tv_usec);
        strcat(out, buff);
        deallocatePtr(buff);
    }
    return out;
}

char *_getInfo(const struct Command *cmd)
{
    if (cmd->argCount == 0)
        return "Usage: get-proc-info <pid>";
    if (!_loggedIn)
        return "This command is available only to logged users, try \"login <user>\"";

    char path[24];
    snprintf(path, 24, "/proc/%s/status", cmd->args[0]);
    if (access(path, F_OK))
        return "Process not found";
    if (access(path, R_OK))
        return "Unable to read";
    FILE *statusFile = fopen(path, "r");

    char *buff = NULL;
    size_t _buffSize;
    char *out = (char *)allocatePtr(sizeof(char), 128);
    _haveAllocated = true;
    while (getline(&buff, &_buffSize, statusFile) != EOF)
        if (strstr(buff, "Name") || strstr(buff, "State") || (strstr(buff, "Pid") && !strstr(buff, "TracerPid")) ||
            strstr(buff, "Uid") || strstr(buff, "VmSize"))
            strcat(out, buff);

    return out;
}

char *_logout(const struct Command *cmd)
{
    if (!_loggedIn)
        return "Already logged out";

    _loggedIn = false;
    strcpy(_username, "");
    return "Logged out";
}

char *_quit(const struct Command *cmd)
{
    return "";
}

char *_help(const struct Command *cmd)
{
    return "Command list:\n\
\tlogin <user> (logs you in as <user> if they exist)\n\
\tget-logged-users (get the users on the system)\n\
\tget-proc-info <pid> (get information about the process with pid <pid>)\n\
\tlogout (logs you out of the current user)\n\
\tquit (?)\n\
\thelp (displays this)";
}
