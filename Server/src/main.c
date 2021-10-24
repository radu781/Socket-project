// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../include/client_communication.h"
#include "../include/input.h"

int main()
{
    int server_fd, new_socket;
    if (establishConnection(&server_fd, &new_socket) < 0)
        return 1;

    for (;;)
    {
        char buffer[1024] = {};
        receiveFromClient(buffer, &new_socket);
        struct Command cmd = validateInput(buffer);
        char *inputResult = executeInput(&cmd, buffer);

        sendToClient(inputResult, &new_socket);
    }
    logCleanUp();
    return 0;
}
