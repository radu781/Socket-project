/* 
    Header that provides functions meant for client to server communication.
    Some refactoring has been done from
    https://www.geeksforgeeks.org/socket-programming-cc/.
*/

#pragma once

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/memory.h"

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
    strcpy(userPrompt + len - 1, userPrompt + len);
    send(*sock, userPrompt, len, 0);
    logComm(stdout, "Client->Server %ld bytes\nmessage:\t%s\n", len - 1, userPrompt);
    fflush(stdin);
}
void receiveFromServer(char *buffer, int *sock)
{
    const size_t len = 10;
    char bytesToGet[len + 1];
    memset(bytesToGet, 0, len + 1);
    read(*sock, bytesToGet, len);

    char tmp[len];
    size_t indexNumber = strstr(bytesToGet, "-=-e") - bytesToGet;
    for (int i = 0; i < indexNumber; i++)
        tmp[i] = bytesToGet[i];
    tmp[indexNumber] = 0;
    size_t sizeOfBuff = atoi(tmp);

    char *dataSent = allocatePtr(sizeof(char), sizeOfBuff + 1);
    strcpy(dataSent, bytesToGet + indexNumber + strlen("-=-e"));

    read(*sock, dataSent + strlen(bytesToGet + indexNumber + strlen("-=-e")), sizeOfBuff);
    logComm(stdout, "Server->Client %ld bytes\nmessage:\t%s\n", strlen(dataSent), dataSent);
    fflush(stdout);
}
