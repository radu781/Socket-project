/* 
    Header that provides functions meant for server to client communication.
    Some refactoring has been done from
    https://www.geeksforgeeks.org/socket-programming-cc/.
*/

#pragma once

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/memory.h"

int establishConnection(int *server_fd, int *new_socket)
{
    const unsigned PORT = 8080;

    logSet("log.txt");
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        logDebug("Socket failed");
        return -1;
    }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        logDebug("setsockopt");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(*server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        logDebug("bind failed");
        return -1;
    }
    if (listen(*server_fd, 3) < 0)
    {
        logDebug("listen");
        return -1;
    }
    if ((*new_socket = accept(*server_fd, (struct sockaddr *)&address,
                              (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        return -1;
    }
    return 0;
}

void sendToClient(const char *data, int *sock)
{
    const size_t len = 10;  
    const char *padding = "-=-e";
    char bytesToSend[len + strlen(padding)];

    snprintf(bytesToSend, len, "%ld", strlen(data));
    strcpy(bytesToSend + strlen(bytesToSend), padding);

    send(*sock, bytesToSend, strlen(bytesToSend), 0);
    fflush(stdout);

    send(*sock, data, strlen(data), 0);
    logComm(stdout, "Server->Client %ld bytes\nmessage:\t%s\n", strlen(data), data);
    fflush(stdout);
}
void receiveFromClient(char *buffer, int *sock)
{
    read(*sock, buffer, 1024);
    logComm(stdout, "Client->Server %ld bytes\nmessage:\t%s\n", strlen(buffer), buffer);
}
