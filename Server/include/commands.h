#pragma once

const size_t commandCount = 6;
const char supportedCommands[6][17] = {
    "login", "get-logged-users", "get-proc-info", "logout", "quit", "help"};

char *login(const struct Command *cmd)
{
    return "";
}

char *getUsers(const struct Command *cmd)
{
    return "";
}

char *getInfo(const struct Command *cmd)
{
    return "";
}

char *logout(const struct Command *cmd)
{
    return "";
}

char *quit(const struct Command *cmd)
{
    return "";
}

char *help(const struct Command *cmd)
{
    return "";
}
