#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "colors.h"

int sockfd;
struct sockaddr_in addr;

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
        bytesSent = sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
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
        socklen_t addr_size = sizeof(addr);
        bytesReceived = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addr_size);
        if (bytesReceived < 0)
        {
            perror("Error in recvfrom");
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
    int port = 6969;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]UDP client socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    pthread_t sendThread, receiveThread;
    pthread_create(&sendThread, NULL, sendData, NULL);
    pthread_create(&receiveThread, NULL, receiveData, NULL);

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    return 0;
}