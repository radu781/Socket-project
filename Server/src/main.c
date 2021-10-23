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
        // receive below
        char serverReply[128] = "Reply from server";
        char buffer[1024] = {};
        read(new_socket, buffer, 1024);
        logDebug("Received [%s] [%ld bytes]\n", buffer, strlen(buffer));

        sendToClient(serverReply, &new_socket);
    }
    logCleanUp();
    return 0;
}
