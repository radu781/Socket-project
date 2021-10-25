#pragma once

#include <stdbool.h>

#define _USERLEN 16

bool _loggedIn = false;
char _username[_USERLEN];
bool _haveAllocated = false;
bool _validCommand = false;
