/*
    Header that provides functions that manage the user input: validation
    and execution itself
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "../../shared/include/memory.h"
#include "../include/user_info.h"

struct Command validateInput()
{
    struct Command *cmd = (struct Command *)allocatePtr(sizeof(struct Command), 1);

    // Find the main command or set a flag to be used later
    const char *delim = ",:?! ";
    char *ptr = strtok(buffer, delim);
    if (ptr != NULL)
    {
        cmd->body = (char *)allocatePtr(sizeof(char), strlen(ptr) + 1);
        strcpy(cmd->body, ptr);
    }
    else
    {
        cmd->argCount = -2;
        return *cmd;
    }

    // Check if the main command exists in the supported command list
    bool found = false;
    for (int i = 0; i < _COMMAND_COUNT && !found; i++)
        if (strcmp(cmd->body, supportedCommands[i]) == 0)
            found = true;

    cmd->argCount = found ? 0 : -1;

    return *cmd;
}

typedef struct _ArgPair
{
    unsigned count;
    unsigned maxLen;
};

struct _ArgPair _countArgs(const char *string, const char *delim)
{
    struct _ArgPair *out = (struct _ArgPair *)allocatePtr(sizeof(struct _ArgPair), 1);
    out->count = out->maxLen = 0;
    char *dummy = (char *)allocatePtr(sizeof(char), strlen(string) + 1);
    strcpy(dummy, string);

    // Count the number of arguments and their maximum length
    char *ptr = strtok(dummy, delim);
    while (ptr != NULL)
    {
        out->count++;
        if (strlen(ptr) > out->maxLen)
            out->maxLen = strlen(ptr);
        ptr = strtok(NULL, delim);
    }

    deallocatePtr(dummy);
    return *out;
}

void _clean(struct Command *cmd)
{
    deallocatePtr(cmd->body);
    deallocatePtrPtr((void **)cmd->args, cmd->argCount);
}

char *_checkCommands(struct Command *cmd)
{
    if (strcmp(cmd->body, supportedCommands[0]) == 0)
    {
        char *out = _login(cmd);
        _clean(cmd);
        return out;
    }
    if (strcmp(cmd->body, supportedCommands[1]) == 0)
    {
        char *out = _register(cmd);
        _clean(cmd);
        return out;
    }
    if (strcmp(cmd->body, supportedCommands[2]) == 0)
    {
        char *out = _getUsers();
        _clean(cmd);
        return out;
    }
    if (strcmp(cmd->body, supportedCommands[3]) == 0)
    {
        char *out = _getInfo(cmd);
        _clean(cmd);
        return out;
    }
    if (strcmp(cmd->body, supportedCommands[4]) == 0)
    {
        char *out = _logout();
        _clean(cmd);
        return out;
    }
    if (strcmp(cmd->body, supportedCommands[5]) == 0)
    {
        char *out = _quit();
        _clean(cmd);
        return out;
    }
    if (strcmp(cmd->body, supportedCommands[6]) == 0)
    {
        char *out = _help();
        _clean(cmd);
        return out;
    }

    return "Unexpected error";
}

char *executeInput(struct Command *cmd)
{
    if (cmd->argCount == -1)
        return "Command not found, please try again";
    if (cmd->argCount == -2)
        return "\",:?! [EOF]\" are not valid commands";

    // Memory hack: ignore the main command to reveal the arguments
    strcpy(buffer, buffer + strlen(buffer) + 1);

    // Allocate memory accordingly
    const char *delim = ",:?! ";
    struct _ArgPair vals = _countArgs(buffer, delim);
    cmd->argCount = vals.count;
    cmd->args = (char **)allocatePtrPtr(sizeof(char), sizeof(char *), cmd->argCount, vals.maxLen);

    // Find and set all arguments
    unsigned index = 0;
    char *ptr = strtok(buffer, delim);
    while (ptr != NULL)
    {
        strcpy(cmd->args[index++], ptr);
        ptr = strtok(NULL, delim);
    }

    return _checkCommands(cmd);
}

char *processInput()
{
    struct Command cmd = validateInput();
    return executeInput(&cmd);
}

void cleanInput(char *input)
{
    if (_haveAllocated)
    {
        deallocatePtr(input);
        _haveAllocated = false;
    }
}
