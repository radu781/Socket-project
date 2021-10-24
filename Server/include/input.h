#pragma once

#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "../../shared/include/memory.h"

struct Command validateInput(char *fromUser)
{
    struct Command *cmd = (struct Command *)allocatePtr(sizeof(struct Command), 1);

    const char *delim = ",:?! ";
    char *ptr = strtok(fromUser, delim);
    if (ptr != NULL)
    {
        cmd->body = (char *)allocatePtr(sizeof(char), strlen(ptr) + 1);
        strcpy(cmd->body, ptr);
    }
    else
    {
        cmd->argCount = -2;
        _validCommand = false;
        return *cmd;
    }

    bool found = false;
    for (int i = 0; i < commandCount && !found; i++)
        if (strcmp(cmd->body, supportedCommands[i]) == 0)
            found = true;

    _validCommand = found;
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

char *executeInput(struct Command *cmd, char *fromUser)
{
    if (cmd->argCount == -1)
        return "Command not found, please try again";
    if (cmd->argCount == -2)
        return "\",:?! [EOF]\" are not valid commands";

    fromUser += strlen(fromUser) + 1;
    const char *delim = ",:?!";
    struct _ArgPair vals = _countArgs(fromUser, delim);
    cmd->argCount = vals.count;
    cmd->args = (char **)allocatePtrPtr(sizeof(char), sizeof(char *), cmd->argCount, vals.maxLen);

    unsigned index = 0;
    char *ptr = strtok(fromUser, delim);
    while (ptr != NULL)
    {
        strcpy(cmd->args[index++], ptr);
        ptr = strtok(NULL, delim);
    }

    if (strcmp(cmd->body, supportedCommands[0]) == 0)
        return _login(cmd);
    if (strcmp(cmd->body, supportedCommands[1]) == 0)
        return _getUsers(cmd);
    if (strcmp(cmd->body, supportedCommands[2]) == 0)
        return _getInfo(cmd);
    if (strcmp(cmd->body, supportedCommands[3]) == 0)
        return _logout(cmd);
    if (strcmp(cmd->body, supportedCommands[4]) == 0)
        return _quit(cmd);
    if (strcmp(cmd->body, supportedCommands[5]) == 0)
        return _help(cmd);

    // deallocatePtr((void *)&vals);
    deallocatePtr((void *)cmd->body);
    deallocatePtrPtr((void **)cmd->args, cmd->argCount);
    // deallocatePtr((void *)cmd);
    return "Unexpected error";
}
