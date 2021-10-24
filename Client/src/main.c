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
        char *buffer = receiveFromServer(&sock);
        printf("%s\n", buffer);
        deallocatePtr(buffer);
    }
    logCleanUp();
    return 0;
}
