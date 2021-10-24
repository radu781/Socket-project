#pragma once

#include "user_info.h"
#include "../../shared/include/logger.h"

const size_t commandCount = 6;
const char supportedCommands[6][17] = {
    "login", "get-logged-users", "get-proc-info", "logout", "quit", "help"};
typedef struct Command
{
    char *body;
    char **args;
    unsigned argCount;
};

char *_login(const struct Command *cmd)
{
    if (_loggedIn)
    {
        char *buff = (char *)allocatePtr(sizeof(char), 43 + 10);
        snprintf(buff, 43 + 10, "Already logged in as %s, try \"logout\" first", _username);
        _haveAllocated = true;
        return buff;
    }
    if (true)
    {
        logComm(stdout, "Logged in as %s", _username);
        _loggedIn = true;
        return "";
    }

    _haveAllocated = false;
    return "Unexpected error";
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
    return "";
}

char *_quit(const struct Command *cmd)
{
    return "";
}

char *_help(const struct Command *cmd)
{
    return "";
}
