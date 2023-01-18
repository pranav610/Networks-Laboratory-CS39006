/*
			NETWORK PROGRAMMING WITH SOCKETS
        Assignment 2 (TCP and UDP Client-Server Programming)
    A. Write a UDP client-server system to make the time server
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <time.h>
#define MAX_SIZE 100
#define MAX_ATTEMPTS 5

            /* The server process*/

int main()
{
    char buff[MAX_SIZE];
    memset(buff, 0, sizeof(buff));

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd < 0)
    {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8181);

    if(bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Server Running\n");

    while(1)
    {   
        printf("Waiting for client's resposnse\n");
        // In order to get ip address of client 1st recv a call from the client
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buff, MAX_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
        if(n==0) 
            break;
        buff[n] = '\0';

        printf("Message recieved from client: %s\n", buff);
        // After getting ip of client from recv call inorder to send local time
        // server will make sendto call to the client

        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        sprintf(buff, "Current local time and date: %s", asctime(timeinfo));

        // send to call
        sendto(sockfd, buff, strlen(buff)+1, 0, (const struct sockaddr *) &cliaddr, len);
    }
}