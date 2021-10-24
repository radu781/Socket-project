/* 
    Header that provides functions meant for client to server communication.
    Some refactoring has been done from
    https://www.geeksforgeeks.org/socket-programming-cc/.
*/

#pragma once

#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "../../shared/include/constants.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/memory.h"

int establishConnection(int *sock)
{
    logSet("log.txt");
    struct sockaddr_in serv_addr;
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket: ");
        logDebug("Socket creation error");
        return -1;
    }
    logDebug("Socket ok");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, localhost, &serv_addr.sin_addr) <= 0)
    {
        perror("Address: ");
        logDebug("Invalid address/Address not supported");
        return -1;
    }
    logDebug("Address ok");
    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection: ");
        logDebug("Connection Failed");
        return -1;
    }

    logDebug("Connected\n");
    return 0;
}

void sendToServer(const char *prompt, int *sock)
{
    printf("%s", prompt);

    char *userPrompt1 = NULL;
    size_t _buffSize;
    ssize_t len1 = getline(&userPrompt1, &_buffSize, stdin);
    if (len1 == -1)
    {
        ssize_t sent = send(*sock, " ", 1, 0);
        checkIO(sent, 1);
        logComm(stdout, "Client->Server %ld bytes\nmessage:\t[EOF]", 1);
        exit(1);
    }
    checkIO(len1, strlen(userPrompt1));
    strcpy(userPrompt1 + len1 - 1, userPrompt1 + len1);

    ssize_t sent = send(*sock, userPrompt1, len1, 0);
    checkIO(sent, strlen(userPrompt1) + 1);
    logComm(stdout, "Client->Server %ld bytes\nmessage:\t%s", len1 - 1, userPrompt1);
    fflush(stdin);
}

char *receiveFromServer(int *sock)
{
    char bytesToGet[firstBufferLen + 1];
    memset(bytesToGet, 0, firstBufferLen + 1);
    ssize_t iread = read(*sock, bytesToGet, firstBufferLen);
    checkIO(iread, strlen(bytesToGet));

    char tmp[firstBufferLen];
    size_t indexNumber = strstr(bytesToGet, padding) - bytesToGet;
    for (int i = 0; i < indexNumber; i++)
        tmp[i] = bytesToGet[i];
    tmp[indexNumber] = 0;
    size_t sizeOfBuff = atoi(tmp);

    char *dataSent = allocatePtr(sizeof(char), sizeOfBuff + 1);
    strcpy(dataSent, bytesToGet + indexNumber + strlen(padding));

    if (sizeOfBuff + strlen(padding) < firstBufferLen)
        return dataSent;
    iread = read(*sock, dataSent + strlen(bytesToGet + indexNumber + strlen(padding)), sizeOfBuff);
    checkIO(iread, strlen(dataSent + strlen(bytesToGet + indexNumber + strlen(padding))));

    logComm(stdout, "Server->Client %ld bytes\nmessage:\t%s\n", strlen(dataSent), dataSent);
    fflush(stdout);

    return dataSent;
}
