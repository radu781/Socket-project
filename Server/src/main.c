// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../include/client_communication.h"

int main()
{
    int server_fd, new_socket;
    if (establishConnection(&server_fd, &new_socket) < 0)
        return 1;

    for (;;)
    {
        char buffer[1024] = {};
        receiveFromClient(buffer, &new_socket);

        sendToClient("01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", &new_socket);
    }
    logCleanUp();
    return 0;
}
