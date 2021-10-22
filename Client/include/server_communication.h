/* 
    Header that provides functions meant for client to server communication.
    Some refactoring has been done from
    https://www.geeksforgeeks.org/socket-programming-cc/.
*/

#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../shared/include/logger.h"

int establishConnection(int *sock)
{
    const char *localhost = "127.0.0.1";
    const unsigned PORT = 8080;

    logSet("log.txt");
    struct sockaddr_in serv_addr;
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        logDebug("Socket creation error");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, localhost, &serv_addr.sin_addr) <= 0)
    {
        logDebug("Invalid address/Address not supported");
        return -1;
    }
    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        logDebug("Connection Failed");
        return -1;
    }

    return 0;
}

void sendToServer(const char *prompt, int *sock)
{
    printf("%s", prompt);

    char *userPrompt = NULL;
    size_t _buffSize;
    ssize_t len = getline(&userPrompt, &_buffSize, stdin);
    send(*sock, userPrompt, len, 0);
    logDebug("Client->Server %ld bytes\nmessage:\t%s", len, userPrompt);
    fflush(stdin);
}
void receiveFromServer()
{
}
