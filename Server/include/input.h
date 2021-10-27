/*
    Header that provides functions that manage the user input: validation
    and execution itself
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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

void _writeAndExit(int fd[2], char *buff, bool registerOffset)
{
    write(fd[1], buff, strlen(buff));

    size_t msglen = strlen(buff) / 100 + (strlen(buff) % 100 != 0);
    if (_loggedIn)
        exit(_usernameIndex + registerOffset + msglen * maxusers);
    else
        exit(msglen * maxusers);
}

char *_checkCommands(struct Command *cmd)
{
    pid_t pid;
    int fd[2];
    if (pipe(fd) == -1)
    {
        logDebug("Pipe creation failed");
        return "Internal error";
    }

    int exitVal = 0;
    switch (pid = fork())
    {
    case -1:
        logDebug("Fork creation failed");
        return "Internal error";
    case 0:
        close(fd[0]);

        if (strcmp(cmd->body, supportedCommands[0]) == 0)
        {
            char *out = _login(cmd);
            _clean(cmd);

            _writeAndExit(fd, out, false);
        }
        if (strcmp(cmd->body, supportedCommands[1]) == 0)
        {
            char *out = _register(cmd);
            _clean(cmd);

            _writeAndExit(fd, out, true);
        }
        if (strcmp(cmd->body, supportedCommands[2]) == 0)
        {
            char *out = _getUsers();
            _clean(cmd);

            _writeAndExit(fd, out, false);
        }
        if (strcmp(cmd->body, supportedCommands[3]) == 0)
        {
            char *out = _getInfo(cmd);
            _clean(cmd);

            _writeAndExit(fd, out, false);
        }
        if (strcmp(cmd->body, supportedCommands[4]) == 0)
        {
            char *out = _logout();
            _clean(cmd);

            _writeAndExit(fd, out, false);
        }
        if (strcmp(cmd->body, supportedCommands[5]) == 0)
        {
            char *out = _quit();
            _clean(cmd);

            _writeAndExit(fd, out, false);
        }
        if (strcmp(cmd->body, supportedCommands[6]) == 0)
        {
            char *out = _help();
            _clean(cmd);

            _writeAndExit(fd, out, false);
        }
    default:
        close(fd[1]);
        wait(&exitVal);
        _loggedIn = (WEXITSTATUS(exitVal) % maxusers != 0);
        size_t uindex = WEXITSTATUS(exitVal) % maxusers;
        if (_loggedIn)
            _findUserAt(uindex);
        size_t buffLen = WEXITSTATUS(exitVal) - uindex;

        char *toRead = (char *)allocatePtr(sizeof(char), buffLen * maxusers + 1);
        read(fd[0], toRead, buffLen * maxusers + 1);
        return toRead;
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
