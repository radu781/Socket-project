#include "../include/client_communication.h"
#include "../include/input.h"

int main()
{
    int server_fd, new_socket;
    if (establishConnection(&server_fd, &new_socket) < 0)
        return 1;

    for (;;)
    {
        receiveFromClient(&new_socket);
        char *result = processInput();

        sendToClient(result, &new_socket);
        cleanInput(result);
        if (_hasQuit)
            break;
    }
    logCleanUp();
    return 0;
}
