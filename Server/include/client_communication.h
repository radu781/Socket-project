/* 
    Header that provides functions meant for server to client communication.
    Some refactoring has been done from
    https://www.geeksforgeeks.org/socket-programming-cc/.
*/

#pragma once

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../include/user_info.h"
#include "../../shared/include/constants.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/memory.h"

int establishConnection(int *server_fd, int *new_socket)
{
    logSet("log.txt");
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket: ");
        logDebug("Socket failed");
        return -1;
    }
    logDebug("Socket ok");
    // Forcefully attaching socket to the port 8080
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("Socket: ");
        logDebug("setsockopt");
        return -1;
    }
    logDebug("Setsockopt ok");
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(*server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind: ");
        logDebug("bind failed");
        return -1;
    }
    logDebug("Bind ok");
    if (listen(*server_fd, 3) < 0)
    {
        perror("Listen: ");
        logDebug("listen");
        return -1;
    }
    logDebug("Listening");
    if ((*new_socket = accept(*server_fd, (struct sockaddr *)&address,
                              (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        logDebug("accept");
        return -1;
    }

    logDebug("Accepted\n");
    return 0;
}

void sendToClient(const char *toSend, int *sock)
{
    char bytesToSend[10];

    // Creates a buffer for the data to be sent to the client and the username
    char *dummy = (char *)allocatePtr(sizeof(char), strlen(toSend) + _USERLEN + 1);
    if (_loggedIn)
        snprintf(dummy, strlen(toSend) + _USERLEN, "%s\n%s> ", toSend, _username);
    else
        snprintf(dummy, strlen(toSend) + _USERLEN, "%s\n", toSend);

    // Adds the data byte size and separates it from the actual message
    snprintf(bytesToSend, firstBufferLen, "%ld", strlen(dummy));
    strcpy(bytesToSend + strlen(bytesToSend), padding);

    // Send the number of bytes
    ssize_t sent = send(*sock, bytesToSend, strlen(bytesToSend), 0);
    checkIO(sent, strlen(bytesToSend));
    fflush(stdout);
    
    // Send the message body
    sent = send(*sock, dummy, strlen(dummy), 0);
    checkIO(sent, strlen(dummy));
    logComm(stdout, "Server->Client %ld bytes\nmessage:\t%s", strlen(dummy), dummy);

    deallocatePtr(dummy);
    fflush(stdout);
}

void receiveFromClient(int *sock)
{
    ssize_t red = read(*sock, buffer, 1024);
    checkIO(red, strlen(buffer) + 1);
    logComm(stdout, "Client->Server %ld bytes\nmessage:\t%s", strlen(buffer), buffer);
}
