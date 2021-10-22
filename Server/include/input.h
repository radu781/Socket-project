#pragma once

#include <stdio.h>
#include <string.h>

#include "../shared/include/memory.h"

typedef struct Command
{
    const size_t maxArgCount;
    const size_t maxArgLen;

    char *body;
    char **args;
    unsigned argCount;
};

struct Command input(const char *prompt)
{
    struct Command *cmd = (struct Command *)allocatePtr(sizeof(struct Command), 1);
    *(size_t *)&cmd->maxArgLen = 20;
    *(size_t *)&cmd->maxArgCount = 4;
    cmd->body = (char *)allocatePtr(sizeof(char), cmd->maxArgLen);
    cmd->args = (char **)allocatePtrPtr(sizeof(char), sizeof(char *), cmd->maxArgLen, cmd->maxArgCount);
    cmd->argCount = -1;

    size_t lineSize;
    char *buffer;
    printf("%s", prompt);
    getline(&buffer, &lineSize, stdin);

    const char *delim = ",:?! ";
    char *ptr = strtok(buffer, delim);
    while (ptr != NULL)
    {
        if (cmd->argCount == -1)
        {
            strcpy(cmd->body, ptr);
            cmd->argCount++;
        }
        else
        {
            strcpy(cmd->args[cmd->argCount], ptr);
            cmd->argCount++;
        }
        ptr = strtok(NULL, delim);
    }
    printf("Your prompt is: %s\n", cmd->body);
    for (int i = 0; i < cmd->argCount; i++)
        printf("%s\n", cmd->args[i]);
    return *cmd;
}
