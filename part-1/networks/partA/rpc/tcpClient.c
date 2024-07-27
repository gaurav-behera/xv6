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
char choice;
char play;

void checkReady()
{
    char buffer[1024];
    bzero(buffer, 1024);
    int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0)
    {
        perror("Error in recv");
        exit(1);
    }
    if (strncasecmp("Ready", buffer, 5) != 0)
    {
        if (play == 'Y')
        {
            printf("Opponent not interest in playing again.\n");
        }
        printf("Terminating Session...\n");
        exit(0);
    }
}

bool isValidInput(char *input)
{
    if (strlen(input) < 1)
    {
        return false;
    }

    if (input[0] == 'R' || input[0] == 'S' || input[0] == 'P' || input[0] == 'r' || input[0] == 's' || input[0] == 'p')
    {
        return true;
    }
    return false;
}

void acceptAndSendInput()
{
    char buffer[1024];
    char inputChoice[1024];
    while (true)
    {
        printf(CYAN_COLOR "Enter your choice - Rock, Paper or Scissors (R,P,S):");
        printf(RESET_COLOR " ");
        bzero(buffer, 1024);
        if (fgets(buffer, 1024, stdin) == NULL)
        {
            continue;
        }
        if (isValidInput(buffer))
        {
            inputChoice[0] = buffer[0];
            inputChoice[1] = '\0';
            if (inputChoice[0] >= 'a')
            {
                inputChoice[0] = inputChoice[0] - 'a' + 'A';
            }
            break;
        }
        else
        {
            printf(RED_COLOR "Invalid Input\n" RESET_COLOR);
        }
    }
    int bytesSent;
    choice = inputChoice[0];
    bytesSent = send(sockfd, inputChoice, 1024, 0);
    if (bytesSent == -1)
    {
        perror("Error in send");
        exit(1);
    }
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

void receiveResult()
{
    char buffer[1024];
    int bytesReceived;

    bzero(buffer, 1024);
    bytesReceived = recv(sockfd, buffer, 1024, 0);
    if (bytesReceived < 0)
    {
        perror("Error in recv");
        exit(1);
    }
    printf("Your choice: %s\n", getActualChoice(choice));
    printf("Opponents choice: %s\n", getActualChoice(buffer[0]));
    printf("%s\n", buffer + 1);
    printf(RESET_COLOR "");
}

void playAgain()
{
    char buffer[1024];
    char inputChoice[1024];
    while (true)
    {
        printf(CYAN_COLOR "Play Again? (Y/N) " RESET_COLOR);
        printf(RESET_COLOR " ");
        bzero(buffer, 1024);
        if (fgets(buffer, 1024, stdin) == NULL)
        {
            continue;
        }
        if (buffer[0] == 'y' || buffer[0] == 'n' || buffer[0] == 'N' || buffer[0] == 'Y')
        {
            inputChoice[0] = buffer[0];
            inputChoice[1] = '\0';
            if (inputChoice[0] >= 'a')
            {
                inputChoice[0] = inputChoice[0] - 'a' + 'A';
            }
            break;
        }
        else
        {
            printf(RED_COLOR "Invalid Input\n" RESET_COLOR);
        }
    }
    int bytesSent;
    bytesSent = send(sockfd, inputChoice, 1024, 0);
    play = inputChoice[0];
    if (bytesSent == -1)
    {
        perror("Error in send");
        exit(1);
    }
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

    struct sockaddr_in addr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP client socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    while (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
    }
    printf(GREEN_COLOR "[+]Joined Game\n");
    printf(RESET_COLOR "");

    while (1)
    {
        checkReady();
        acceptAndSendInput();
        receiveResult();
        playAgain();
    }

    return 0;
}