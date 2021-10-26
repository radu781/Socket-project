#include <unistd.h>
#include <string.h>
#include "../include/server_communication.h"

int main()
{
    int sock = 0;
    if (establishConnection(&sock) < 0)
        return 1;

    printf("Use \"help\" for command info\n\n");
    for (;;)
    {
        sendToServer("", &sock);
        char *buffer = receiveFromServer(&sock);
        if (_quit)
            break;
        printf("%s", buffer);
        deallocatePtr(buffer);
    }
    logCleanUp();
    return 0;
}
