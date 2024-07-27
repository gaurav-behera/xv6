#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "colors.h"

int sock;

void *sendData(void *args)
{
    char buffer[1024];
    int bytesSent;
    while (1)
    {
        bzero(buffer, 1024);
        if (fgets(buffer, 1024, stdin) == NULL)
        {
            continue;
        }
        bytesSent = send(sock, buffer, strlen(buffer), 0);
        if (bytesSent == -1)
        {
            perror("Error in sendto");
            exit(1);
        }
        printf(RED_COLOR "[+]Data sent: %s", buffer);
        printf(RESET_COLOR "");
    }
    return NULL;
}

void *receiveData(void *args)
{
    char buffer[1024];
    int bytesReceived;
    while (1)
    {
        bzero(buffer, 1024);
        bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0)
        {
            perror("Error in recv");
            exit(1);
        }

        printf(GREEN_COLOR "[+]Data recv: %s", buffer);
        printf(RESET_COLOR "");
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Invalid Usage!");
        exit(0);
    }

    char *ip = "127.0.0.1";
    int port = 6970;

    struct sockaddr_in addr;
    socklen_t addr_size;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP client socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    printf("Connecting to server...\n");
    while (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
    }
    printf("Connected to the server.\n");

    pthread_t sendThread, receiveThread;
    pthread_create(&sendThread, NULL, sendData, NULL);
    pthread_create(&receiveThread, NULL, receiveData, NULL);

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    // close(sock);
    // printf("Disconnected from the server.\n");

    return 0;
}