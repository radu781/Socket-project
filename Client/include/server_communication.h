/* 
    Header that provides functions meant for client to server communication.
    Some refactoring has been done from
    https://www.geeksforgeeks.org/socket-programming-cc/.
*/

#pragma once

#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../../shared/include/constants.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/memory.h"

bool _quit = false;

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

void sendToServer(int *sock)
{
    // Read user input and check if it is EOF
    char *userPrompt = NULL;
    size_t _buffSize;
    ssize_t len1 = getline(&userPrompt, &_buffSize, stdin);
    if (len1 == -1)
    {
        ssize_t sent = send(*sock, " ", 1, 0);
        checkIO(sent, 1);
        logComm(stdout, "Client->Server %ld bytes\nmessage:\t[EOF]", 1);
        exit(1);
    }
    checkIO(len1, strlen(userPrompt));
    strcpy(userPrompt + len1 - 1, userPrompt + len1);

    // Send the input to the server
    ssize_t sent = send(*sock, userPrompt, len1, 0);
    checkIO(sent, strlen(userPrompt) + 1);
    logComm(stdout, "Client->Server %ld bytes\nmessage:\t%s", len1 - 1, userPrompt);
    deallocatePtr(userPrompt);
    fflush(stdin);
}

bool _userQuit(const char *reply)
{
    char *quitTest = (char *)allocatePtr(sizeof(char), 6 + strlen(padding));

    snprintf(quitTest, 6 + strlen(padding), "quit%s\n", padding);
    if (strstr(reply, quitTest))
    {
        logComm(stdout, "Client quit");
        _quit = true;
    }

    return _quit;
}

char *receiveFromServer(int *sock)
{
    // Read first chunk of data
    char bytesToGet[firstBufferLen + 1];
    memset(bytesToGet, 0, firstBufferLen + 1);
    ssize_t iread = read(*sock, bytesToGet, firstBufferLen);
    checkIO(iread, strlen(bytesToGet));

    // Determine the size of the reply
    char tmp[firstBufferLen];
    size_t indexNumber = strstr(bytesToGet, padding) - bytesToGet;
    for (int i = 0; i < indexNumber; i++)
        tmp[i] = bytesToGet[i];
    tmp[indexNumber] = 0;
    size_t sizeOfBuff = atoi(tmp);

    // Recreate the message starting from index of size + padding
    char *dataSent = allocatePtr(sizeof(char), sizeOfBuff + 1);
    strcpy(dataSent, bytesToGet + indexNumber + strlen(padding));

    // Check if another read is necessary
    if (sizeOfBuff + strlen(padding) < firstBufferLen)
        return dataSent;
    size_t offset = strlen(bytesToGet + indexNumber + strlen(padding));
    iread = read(*sock, dataSent + offset, sizeOfBuff);
    checkIO(iread, strlen(dataSent + offset));

    // Check if the client issued the quit command
    if (_userQuit(dataSent))
    {
        logComm(stdout, "Client quit");
        return dataSent;
    }
    logComm(stdout, "Server->Client %ld bytes\nmessage:\t%s\n", strlen(dataSent), dataSent);
    fflush(stdout);

    return dataSent;
}

void display(char *buff)
{
    printf("%s", buff);
    deallocatePtr(buff);
}
