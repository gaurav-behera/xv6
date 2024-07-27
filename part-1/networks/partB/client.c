#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
#include "colors.h"

#define CHUNK_SIZE 5

int sockfd;
struct sockaddr_in addr;

typedef struct dataChunk
{
    int totalChunks;
    int seqNum;
    char data[CHUNK_SIZE + 1];
    int ack;
} dataChunk;

typedef struct dataStruct
{
    dataChunk **chunks;
    int chunkCount;
} dataStruct;
dataStruct *transferData = NULL;

dataChunk *makedataChunk(char *data, int ack, int seq, int totalChunks)
{
    dataChunk *d = malloc(sizeof(dataChunk));
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        d->data[i] = data[i];
        if (data[i] == '\0')
        {
            break;
        }
    }
    d->data[CHUNK_SIZE] = '\0';
    d->ack = ack;
    d->seqNum = seq;
    d->totalChunks = totalChunks;
    return d;
}

void printEntireData(dataStruct *d)
{
    for (int i = 0; i < d->chunkCount; i++)
    {
        printf("%s", d->chunks[i]->data);
    }
    printf("\n");
    printf(RESET_COLOR "");
}

dataStruct *splitInput(char *input)
{
    int totalChunks = strlen(input) / CHUNK_SIZE;
    if (totalChunks * CHUNK_SIZE != strlen(input))
    {
        totalChunks++;
    }
    dataChunk **chunks = malloc(sizeof(dataChunk *) * totalChunks);
    int chunkCount = 0;
    for (int i = 0; i < strlen(input); i += 5)
    {
        chunks[chunkCount] = makedataChunk(input + i, 0, chunkCount, totalChunks);
        chunkCount++;
    }
    dataStruct *d = malloc(sizeof(dataStruct));
    d->chunks = chunks;
    d->chunkCount = chunkCount;
    // for (int i = 0; i < chunkCount; i++)
    // {
    //     printf("%s %d\n", d->chunks[i]->data, d->chunks[i]->ack);
    // }
    return d;
}

int getNextNonAck(dataStruct *data, int currentIdx)
{
    // returns -1 if all are acknowdledged or else returns the first non acknowledged index greater than currentIdx
    int retVal = -1;
    int ackCount = 0;
    for (int i = currentIdx; i < data->chunkCount; i++)
    {
        if (data->chunks[i] == NULL || data->chunks[i]->ack == 0)
        {
            retVal = i;
            break;
        }
        else
        {
            ackCount++;
        }
        if (ackCount == data->chunkCount)
        {
            break;
        }

        if (i == data->chunkCount - 1)
        {
            i = -1;
        }
    }
    return retVal;
}

void emptyBuffer()
{
    char buffer[1024];
    socklen_t addr_size = sizeof(addr);
    ssize_t bytesRead;

    while (1)
    {
        bytesRead = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_size);
        if (bytesRead == -1 || bytesRead == 0)
        {
            break;
        }
    }
}

int receiveAck()
{
    char buffer[1024];

    socklen_t addr_size = sizeof(addr);
    int bytesRecieved = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_size);
    if (bytesRecieved == 0 || bytesRecieved == -1)
    {
        return 0;
    }

    int seq = atoi(buffer + 4);
    if (seq == -1)
    {
        printf(GREEN_COLOR "[+]Entire data has been sucessfully sent\n");
        printf(RESET_COLOR "");
        return 1;
    }

    if (transferData->chunks[seq]->ack == 1)
    {
        return 0;
    }
    transferData->chunks[seq]->ack = 1;
    printf(GREEN_COLOR "[+]Received acknowledgment for chunk %d\n", seq);
    printf(RESET_COLOR "");
    return 0;
}

void freeTransferData()
{
    dataStruct *d = transferData;
    for (int i = 0; i < d->chunkCount; i++)
    {
        free(d->chunks[i]);
    }
    free(d->chunks);
    free(d);
    transferData = NULL;
}

void sendChunk(struct dataChunk *chunk)
{
    char chunkBuffer[sizeof(dataChunk)];
    memcpy(chunkBuffer, chunk, sizeof(dataChunk));
    // send(sockfd, chunkBuffer, sizeof(dataChunk), 0);
    sendto(sockfd, chunkBuffer, sizeof(dataChunk), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (chunk->seqNum >= 0)
    {
        printf(CYAN_COLOR "[+]Chunk %d sent\n", chunk->seqNum);
    }
}

void *sendData(void *args)
{
    while (1)
    {
        char buffer[1024];
        bzero(buffer, 1024);
        if (fgets(buffer, 1024, stdin) == NULL)
        {
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';

        transferData = splitInput(buffer);

        int currentChunk = 0;

        time_t sendTimes[transferData->chunkCount];
        int sendOnce = 0;
        time_t lastSendTime;

        while (getNextNonAck(transferData, 0) != -1)
        {
            if (sendOnce == 0)
            {
                sendChunk(transferData->chunks[currentChunk]);
                sendTimes[currentChunk] = clock();
                lastSendTime = clock();
            }

            // retransmission
            time_t currentTime = clock();
            for (int i = 0; i < transferData->chunkCount; i++)
            {
                if (sendTimes[i] != 0 && (((double)(clock() - sendTimes[i])) / CLOCKS_PER_SEC) > 2)
                {
                    receiveAck();
                    if (transferData->chunks[i]->ack == 0)
                    {
                        sendChunk(transferData->chunks[i]);
                        sendTimes[i] = clock();
                    }
                }
            }
            if (sendOnce == 0)
            {
                currentChunk++;
                if (currentChunk == transferData->chunkCount)
                {
                    sendOnce = 1;
                }
            }
            while ((double)(clock() - lastSendTime) / CLOCKS_PER_SEC < 1)
            {
            }
        }
        // fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) & ~O_NONBLOCK);
        // mark entire data sent by sending a chunk with seq = -1
        struct dataChunk *endChunk = makedataChunk("", 0, -1, 0);
        sendChunk(endChunk);
        // fcntl(sockfd, F_SETFL, O_NONBLOCK);
        while (receiveAck() != 1)
        {
            sendChunk(endChunk);
        }
        // fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) & ~O_NONBLOCK);
        printf(RESET_COLOR "");
        freeTransferData();
        break;
    }
    emptyBuffer();

    return NULL;
}

void sendAck(int seqNum)
{
    char buffer[1024];
    strcpy(buffer, "ACK:");
    char seqStr[1024];
    sprintf(seqStr, "%d", seqNum);
    strcpy(buffer + strlen(buffer), seqStr);
    int byetsSent = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
}

void *receiveData(void *args)
{
    while (1)
    {
        dataChunk d;
        char buffer[sizeof(dataChunk)];
        socklen_t addr_size = sizeof(addr);
        ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        // ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_size);

        if (bytes_received == sizeof(dataChunk))
        {
            memcpy(&d, buffer, sizeof(dataChunk));
        }
        else
        {
            continue;
        }

        // if (rand() % 10 < 5)
        // {
        //     continue;
        // }
        // ignore if it's the acknowledgement message
        // ignore if it's the acknowledgement message
        if (strncmp(buffer, "ACK:", 4) == 0)
        {
            // int seq = atoi(buffer + 4);
            // if (seq == -1)
            // {
            //     printf(GREEN_COLOR "[+]Entire data has been sucessfully sent\n");
            //     printf(RESET_COLOR "");
            //     // return 1;
            //     continue;
            // }

            // if (transferData->chunks[seq]->ack == 1)
            // {
            //     // return 0;
            //     continue;
            // }
            // transferData->chunks[seq]->ack = 1;
            // printf(GREEN_COLOR "[+]Received acknowledgment for chunk %d\n", seq);
            // printf(RESET_COLOR "");
            // return 0;
            continue;
        }

        sendAck(d.seqNum);

        if (d.seqNum == -1)
        {
            if (transferData == NULL)
            {
                continue;
            }
            printf(GREEN_COLOR "[+]Data recieved: " RESET_COLOR);
            printEntireData(transferData);
            freeTransferData();
            transferData = NULL;
            break;
        }
        else
        {
            if (transferData == NULL)
            {
                transferData = malloc(sizeof(dataStruct));
                transferData->chunkCount = d.totalChunks;
                transferData->chunks = malloc(sizeof(dataChunk *) * d.totalChunks);
            }
            transferData->chunks[d.seqNum] = makedataChunk(d.data, 1, d.seqNum, d.totalChunks);
        }

        printf(RESET_COLOR "");
    }
    return NULL;
}

void joinServer()
{
    char buffer[1024];
    int bytesSent;
    bzero(buffer, 1024);
    strcpy(buffer, "JOIN");
    bytesSent = sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (bytesSent == -1)
    {
        perror("Error in sendto");
        exit(1);
    }
    printf(GREEN_COLOR "[+]Joined Server\n");
    printf(RESET_COLOR "");
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

    joinServer();

    pthread_t sendThread, receiveThread;
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // pthread_create(&receiveThread, NULL, receiveData, NULL);
    // pthread_create(&sendThread, NULL, sendData, NULL);

    // pthread_join(receiveThread, NULL);
    // pthread_join(sendThread, NULL);
    while (1)
    {
        receiveData(NULL);
        sendData(NULL);
        emptyBuffer();
    }
    return 0;
}