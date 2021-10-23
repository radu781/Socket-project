// Client side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <string.h>
#include "../include/server_communication.h"

int main()
{
    int sock = 0;
    if (establishConnection(&sock) < 0)
        return 1;

    for (;;)
    {
        sendToServer("Enter message to send: ", &sock);
        char buffer[1024] = {};
        receiveFromServer(buffer, &sock);
        printf("%s", buffer);
    }
    logCleanUp();
    return 0;
}
