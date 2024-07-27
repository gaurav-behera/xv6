#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "colors.h"

int client_sock;

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
        bytesSent = send(client_sock, buffer, strlen(buffer), 0);
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
        bytesReceived = recv(client_sock, buffer, sizeof(buffer), 0);
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
    int port = 6970;

    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    listen(server_sock, 1);
    printf("Listening...\n");

    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Client connected.\n");

    pthread_t sendThread, receiveThread;
    pthread_create(&receiveThread, NULL, receiveData, NULL);
    pthread_create(&sendThread, NULL, sendData, NULL);

    pthread_join(receiveThread, NULL);
    pthread_join(sendThread, NULL);

    // close(client_sock);
    // printf("[+]Client disconnected.\n");

    return 0;
}