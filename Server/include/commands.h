/*
    Header with functions that actually process the client input
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <utmp.h>
#include "user_info.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/memory.h"
#include "../../shared/include/constants.h"

#define _COMMAND_COUNT 7

const char supportedCommands[_COMMAND_COUNT][17] = {
    "login", "register", "get-logged-users", "get-proc-info", "logout", "quit", "help"};
const char *_configFile = ".config";

bool _hasQuit = false;

typedef struct Command
{
    char *body;
    char **args;
    unsigned argCount;
};

/**
 * Returns true if the user is in the config file, also used by the parent process
 * to remember the user name
 */
bool _findUser(const char *username)
{
    // Attempt to open the config file in read mode
    if (access(_configFile, F_OK))
    {
        logDebug("Can't create config file %s", _configFile);
        return false;
    }
    if (access(_configFile, R_OK))
    {
        logDebug("Can't read config file %s", _configFile);
        return false;
    }
    FILE *config = fopen(_configFile, "r");

    char buff[_USERLEN];

    size_t tempLocation = 0;
    while (fscanf(config, "%s", buff) != EOF)
    {
        tempLocation++;
        if (strcmp(buff, username) == 0)
        {
            _usernameIndex = tempLocation;
            fclose(config);
            return true;
        }
    }

    fclose(config);
    return false;
}

void _findUserAt(size_t index)
{
    _usernameIndex = index;

    // Attempt to open the config file in read mode
    if (access(_configFile, F_OK))
    {
        logDebug("Can't create config file %s", _configFile);
        return;
    }
    if (access(_configFile, R_OK))
    {
        logDebug("Can't read config file %s", _configFile);
        return;
    }
    FILE *config = fopen(_configFile, "r");

    char buff[_USERLEN];

    for (int i = 0; i < index; i++)
        fscanf(config, "%s", buff);
    strcpy(_username, buff);

    fclose(config);
}

char *_login(const struct Command *cmd)
{
    if (cmd->argCount == 0)
        return "Usage: login <user>";

    if (_loggedIn)
    {
        _findUser(cmd->args[0]);
        char *buff = (char *)allocatePtr(sizeof(char), 47 + _USERLEN);
        _haveAllocated = true;

        snprintf(buff, 47 + _USERLEN, "Already logged in as %s, try \"logout\" first", _username);

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

char *_register(const struct Command *cmd)
{
    if (cmd->argCount == 0)
        return "Usage: register <user>";

    if (_loggedIn)
    {
        _findUser(_username);

        char *buff = (char *)allocatePtr(sizeof(char), 47 + _USERLEN);
        _haveAllocated = true;

        snprintf(buff, 47 + _USERLEN, "Already logged in as %s, try \"logout\" first", _username);

        return buff;
    }
    if (!_findUser(cmd->args[0]))
    {
        // Attempt to open the config file in append mode
        if (access(_configFile, F_OK))
        {
            logDebug("Can't open config file %s", _configFile);
            return "Internal error";
        }
        if (access(_configFile, W_OK))
        {
            logDebug("Can't write to config file %s", _configFile);
            return "Internal error";
        }
        FILE *config = fopen(_configFile, "a");

        // Appends new user to the config file and logs the client in as that user
        fprintf(config, "%s\n", cmd->args[0]);
        strcpy(_username, cmd->args[0]);
        logComm(stdout, "Registered new user: %s", _username);
        _loggedIn = true;

        fclose(config);
        return "";
    }

    return "User already exists, try \"login <user>\"";
}

char *_getUsers()
{
    if (!_loggedIn)
        return "This command is available only to logged users, try \"login <user>\"";

    char *out = (char *)allocatePtr(sizeof(char), 256 * 12);
    _haveAllocated = true;

    _findUser(_username);

    // Loop though the utmp file and add formatted contents to a buffer
    for (;;)
    {
        struct utmp *ut = getutent();
        if (ut == NULL)
            break;

        char buff[256];
        snprintf(buff, 256, "user: %s\nhost: %s\nseconds: %d\nmicrosec: %d\n\n",
                 ut->ut_user, ut->ut_host, ut->ut_tv.tv_sec, ut->ut_tv.tv_usec);
        strcat(out, buff);
    }
    return out;
}

char *_getInfo(const struct Command *cmd)
{
    if (cmd->argCount == 0)
        return "Usage: get-proc-info <pid>";
    if (!_loggedIn)
        return "This command is available only to logged users, try \"login <user>\"";

    _findUser(_username);
    char *out = (char *)allocatePtr(sizeof(char), cmd->argCount * 128);
    for (int i = 0; i < cmd->argCount; i++)
    {
        char path[24];

        // Attempt to open the status file
        snprintf(path, 24, "/proc/%s/status", cmd->args[i]);
        if (access(path, F_OK))
        {
            char buff[64];
            snprintf(buff, 64, "Process <%s> not found", cmd->args[i]);
            strcat(out, buff);
            continue;
        }
        if (access(path, R_OK))
        {
            char buff[64];
            snprintf(buff, 64, "Unable to read process file <%s>", cmd->args[i]);
            strcat(out, buff);
            continue;
        }
        FILE *statusFile = fopen(path, "r");

        _haveAllocated = true;

        // Go through the file and find the data needed
        char *buff = NULL;
        size_t _buffSize;
        while (getline(&buff, &_buffSize, statusFile) != EOF)
            if (strstr(buff, "Name") || strstr(buff, "State") || (strstr(buff, "Pid") && !strstr(buff, "TracerPid")) ||
                strstr(buff, "Uid") || strstr(buff, "VmSize"))
                strcat(out, buff);
        strcat(out, "-=-=-\n");
    }

    return out;
}

char *_logout()
{
    if (!_loggedIn)
        return "Already logged out";

    _findUser(_username);

    _usernameIndex = 0;
    _loggedIn = false;
    strcpy(_username, "");
    return "Logged out";
}

char *_quit()
{
    _findUser(_username);
    char *buff = (char *)allocatePtr(sizeof(char), 5 + strlen(padding));
    snprintf(buff, 5 + strlen(padding), "quit%s", padding);
    _haveAllocated = true;
    _hasQuit = true;
    return buff;
}

char *_help()
{
    return "Command list:\n\
\tlogin <user> (logs you in as <user> if they exist)\n\
\tregister <user> (creates new account if it doesn't exist)\n\
\tget-logged-users (get the users on the system)\n\
\tget-proc-info <pid> (get information about the process with pid <pid>)\n\
\tlogout (logs you out of the current user)\n\
\tquit (closes the client and server)\n\
\thelp (displays this)";
}
