// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "../../shared/include/memory.h"

const unsigned PORT = 8080;

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    for (;;)
    {
        char *userPrompt = NULL;
        size_t buffSize;
        printf("Enter message to send: ");
        ssize_t len = getline(&userPrompt, &buffSize, stdin);
        send(sock, userPrompt, len, 0);

        // printf("Sent [%s] [%ld bytes]\n", len, userPrompt);
        valread = read(sock, buffer, 1024);
        printf("Received [%s] [%ld bytes]\n", buffer, strlen(buffer));
        fflush(stdin);
    }
    return 0;
}
