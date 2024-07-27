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

int client_sock1, client_sock2;

char choice1, choice2;
bool play1 = true, play2 = true;

void startGame(char *string)
{
    int bytesSent;
    bytesSent = send(client_sock1, string, 1024, 0);
    if (bytesSent == -1)
    {
        perror("Error in send");
        exit(1);
    }
    bytesSent = send(client_sock2, string, 1024, 0);
    if (bytesSent == -1)
    {
        perror("Error in send");
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
        bytesSent = send(client_sock1, buffer, 1024, 0);
    }
    if (player == 2)
    {
        bytesSent = send(client_sock2, buffer, 1024, 0);
    }
    if (bytesSent == -1)
    {
        perror("Error in send");
        exit(1);
    }
}

void *receiveData1(void *args)
{
    char buffer[1024];
    int bytesReceived;
    bzero(buffer, 1024);
    bytesReceived = recv(client_sock1, buffer, 1024, 0);
    if (bytesReceived < 0)
    {
        perror("Error in recv");
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
    bytesReceived = recv(client_sock2, buffer, 1024, 0);
    if (bytesReceived < 0)
    {
        perror("Error in recv");
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
    bytesReceived = recv(client_sock1, buffer, 1024, 0);
    if (bytesReceived < 0)
    {
        perror("Error in recv");
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
    bytesReceived = recv(client_sock2, buffer, 1024, 0);
    if (bytesReceived < 0)
    {
        perror("Error in recv");
        exit(1);
    }
    play2 = buffer[0] == 'Y' ? true : false;
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

    int server_sock;
    struct sockaddr_in server_addr, client_addr1, client_addr2;
    socklen_t addr_size1, addr_size2;

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

    printf("Listening...\n");
    if(listen(server_sock, 2) < 0){
        perror("[-]Listem failed");
        exit(EXIT_FAILURE);
    };

    printf(CYAN_COLOR "Waiting for participants to join.\n" RESET_COLOR);
    addr_size1 = sizeof(client_addr1);
    client_sock1 = accept(server_sock, (struct sockaddr *)&client_addr1, &addr_size1);
    printf(GREEN_COLOR "[+]Player 1 has joined\n");
    printf(RESET_COLOR "");

    addr_size2 = sizeof(client_addr2);
    client_sock2 = accept(server_sock, (struct sockaddr *)&client_addr2, &addr_size2);
    printf(GREEN_COLOR "[+]Player 2 has joined\n");
    printf(RESET_COLOR "");

    printf(CYAN_COLOR "Both players have joined. Starting Game now...\n" RESET_COLOR);

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