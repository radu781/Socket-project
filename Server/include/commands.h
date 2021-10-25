#pragma once

#include <stdio.h>
#include "user_info.h"
#include "../../shared/include/logger.h"

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
    return "";
}

char *_getInfo(const struct Command *cmd)
{
    return "";
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
