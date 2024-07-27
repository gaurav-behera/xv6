#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "colors.h"

int sockfd;
struct sockaddr_in client_addr1, client_addr2;
char choice1, choice2;
bool play1 = true, play2 = true;

void startGame(char *string)
{
    int bytesSent;
    bytesSent = sendto(sockfd, string, 1024, 0, (struct sockaddr *)&client_addr1, sizeof(client_addr1));
    if (bytesSent == -1)
    {
        perror("Error in sendto");
        exit(1);
    }
    bytesSent = sendto(sockfd, string, 1024, 0, (struct sockaddr *)&client_addr2, sizeof(client_addr2));
    if (bytesSent == -1)
    {
        perror("Error in sendto");
        exit(1);
    }
}

void sendResult(int player, char choice, char *result)
{
    char buffer[1024];
    int bytesSent;
    bzero(buffer, 1024);
    buffer[0] = choice;
    strcpy(buffer + 1, result);
    if (player == 1)
    {
        bytesSent = sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr1, sizeof(client_addr1));
    }
    if (player == 2)
    {
        bytesSent = sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr2, sizeof(client_addr2));
    }
    if (bytesSent == -1)
    {
        perror("Error in sendto");
        exit(1);
    }
}

void acceptParticipant(int num)
{
    char buffer[1024];
    int bytesReceived;

    bzero(buffer, 1024);
    if (num == 1)
    {
        socklen_t addr_size = sizeof(client_addr1);
        bytesReceived = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr1, &addr_size);
    }
    if (num == 2)
    {
        socklen_t addr_size = sizeof(client_addr2);
        bytesReceived = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr2, &addr_size);
    }

    if (bytesReceived < 0)
    {
        perror("Error in recvfrom");
        exit(1);
    }

    printf(GREEN_COLOR "[+]Player %d has joined\n", num);
    printf(RESET_COLOR "");
}

void *receiveData1(void *args)
{
    char buffer[1024];
    int bytesReceived;
    bzero(buffer, 1024);
    socklen_t addr_size = sizeof(client_addr1);
    bytesReceived = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr1, &addr_size);
    if (bytesReceived < 0)
    {
        perror("Error in recvfrom");
        exit(1);
    }
    choice1 = buffer[0];
    return NULL;
}

void *receiveData2(void *args)
{
    char buffer[1024];
    int bytesReceived;
    bzero(buffer, 1024);
    socklen_t addr_size = sizeof(client_addr2);
    bytesReceived = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr2, &addr_size);
    if (bytesReceived < 0)
    {
        perror("Error in recvfrom");
        exit(1);
    }
    choice2 = buffer[0];
    return NULL;
}

char *getActualChoice(char c)
{
    if (c == 'R')
    {
        return "Rock";
    }
    if (c == 'P')
    {
        return "Paper";
    }
    if (c == 'S')
    {
        return "Scissor";
    }
    return "";
}

void decideWinner()
{
    int winner = 0;
    if (choice1 == choice2)
    {
        winner = 0;
    }
    if ((choice1 == 'P' && choice2 == 'R') || (choice1 == 'S' && choice2 == 'P') || (choice1 == 'R' && choice2 == 'S'))
    {
        winner = 1;
    }
    if ((choice1 == 'P' && choice2 == 'S') || (choice1 == 'S' && choice2 == 'R') || (choice1 == 'R' && choice2 == 'P'))
    {
        winner = 2;
    }
    if (winner == 0)
    {
        printf(YELLOW_COLOR "Player 1: %s\n", getActualChoice(choice1));
        printf(YELLOW_COLOR "Player 2: %s\n", getActualChoice(choice2));
        printf(CYAN_COLOR "Draw\n" RESET_COLOR);
        sendResult(1, choice2, YELLOW_COLOR "Game was a DRAW");
        sendResult(2, choice1, YELLOW_COLOR "Game was a DRAW");
    }
    if (winner == 1)
    {
        printf(GREEN_COLOR "Player 1: %s\n", getActualChoice(choice1));
        printf(RED_COLOR "Player 2: %s\n", getActualChoice(choice2));
        printf(CYAN_COLOR "Player 1 won\n" RESET_COLOR);
        sendResult(1, choice2, GREEN_COLOR "You WON");
        sendResult(2, choice1, RED_COLOR "You LOST");
    }
    if (winner == 2)
    {
        printf(RED_COLOR "Player 1: %s\n", getActualChoice(choice1));
        printf(GREEN_COLOR "Player 2: %s\n", getActualChoice(choice2));
        printf(CYAN_COLOR "Player 2 won\n" RESET_COLOR);
        sendResult(1, choice2, RED_COLOR "You LOST");
        sendResult(2, choice1, GREEN_COLOR "You WON");
    }
}

void *playAgain1(void *args)
{
    char buffer[1024];
    int bytesReceived;
    bzero(buffer, 1024);
    socklen_t addr_size = sizeof(client_addr1);
    bytesReceived = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr1, &addr_size);
    if (bytesReceived < 0)
    {
        perror("Error in recvfrom");
        exit(1);
    }
    play1 = buffer[0] == 'Y' ? true : false;
    return NULL;
}

void *playAgain2(void *args)
{
    char buffer[1024];
    int bytesReceived;
    bzero(buffer, 1024);
    socklen_t addr_size = sizeof(client_addr2);
    bytesReceived = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr2, &addr_size);
    if (bytesReceived < 0)
    {
        perror("Error in recvfrom");
        exit(1);
    }
    play2 = buffer[0] == 'Y' ? true : false;
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 1)
    {
        printf("Invalid Usage!");
        exit(0);
    }

    char *ip = "127.0.0.1";
    int port = 6969;

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]UDP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }

    char buffer[1024];
    int bytesSent, bytesReceived;

    // wait for both participants to join
    printf(CYAN_COLOR "Waiting for participants to join.\n" RESET_COLOR);
    acceptParticipant(1);
    acceptParticipant(2);
    printf(CYAN_COLOR "Both players have joined. Starting Game now...\n" RESET_COLOR);

    // recieve inputs
    while (1)
    {
        if (play1 == true && play2 == true)
        {
            startGame("Ready");

            pthread_t receiveThread1, receiveThread2;
            pthread_create(&receiveThread1, NULL, receiveData1, NULL);
            pthread_create(&receiveThread2, NULL, receiveData2, NULL);

            pthread_join(receiveThread1, NULL);
            pthread_join(receiveThread2, NULL);

            decideWinner();
            printf(CYAN_COLOR"Asking players for rematch\n"RESET_COLOR);

            pthread_t playAgainThread1, playAgainThread2;
            pthread_create(&playAgainThread1, NULL, playAgain1, NULL);
            pthread_create(&playAgainThread2, NULL, playAgain2, NULL);

            pthread_join(playAgainThread1, NULL);
            pthread_join(playAgainThread2, NULL);
        }
        else
        {
            startGame("");
            if (play1 == false)
            {
                printf("Player 1 not interested in playing again.\n");
            }
            if (play2 == false)
            {
                printf("Player 2 not interested in playing again.\n");
            }
            printf("Terminating Session...\n");
            exit(0);
        }
    }

    return 0;
}
