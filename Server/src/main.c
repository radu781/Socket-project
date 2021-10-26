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
        struct Command cmd = validateInput();
        char *inputResult = executeInput(&cmd);

        sendToClient(inputResult, &new_socket);
        if (_haveAllocated)
        {
            deallocatePtr(inputResult);
            _haveAllocated = false;
        }
        if (_hasQuit)
            break;
    }
    logCleanUp();
    return 0;
}
