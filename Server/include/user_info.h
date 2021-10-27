/*
    Header for client related variables that are shared across files
*/

#pragma once

#include <stdbool.h>
#include <unistd.h>
#define _USERLEN 16

bool _loggedIn = false;
char _username[_USERLEN];
size_t _usernameIndex = 0;
// Don't forget to free the command output 
bool _haveAllocated = false;
// Used for receiving the client input
char buffer[1024];
