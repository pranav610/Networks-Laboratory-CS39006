/*
			NETWORK PROGRAMMING WITH SOCKETS
        Assignment 2 (TCP and UDP Client-Server Programming)
    A. Write a UDP client-server system to make the time server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#define MAX_SIZE 100
#define TIMEOUT 3000
#define MAX_ATTEMPTS 5

            /* The Client Process*/

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    char buff[MAX_SIZE];
    memset(buff, 0, sizeof(buff));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8181);
    inet_aton("127.0.0.1", &servaddr.sin_addr); 

    int n;
    socklen_t len = sizeof(servaddr);
    char hello[15] = "Hello Server";

    int count = MAX_ATTEMPTS;

    nfds_t sz = 1;
    struct pollfd fdset[1];
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;
    
    int ret;

    while(count--)
    {       
        printf("Connecting to server...\n");
        sendto(sockfd, hello, strlen(hello)+1, 0, (const struct sockaddr*) &servaddr, len);
        ret = poll(fdset, sz, TIMEOUT);
        if(ret < 0)
        {
            perror("Error in timeout\n");
            exit(EXIT_FAILURE);
        }

        // ret value 0 implies connection closed due to timeout
        if(ret==0)
            continue;

        // as only one socket is provided poll will return 1 on recieving data
        if(ret == 1)
        {   
            // if data is received at client side then only use recieve 
            if(fdset[0].revents==POLLIN)
            {
                recvfrom(sockfd, buff, MAX_SIZE, 0, (struct sockaddr *) &servaddr, &len);
                printf("%s\n", buff);
                close(sockfd);
                exit(EXIT_SUCCESS);
            }
        }
    }

    // timeout limit exceeded
    printf("Timeout exceeded...\n");
    close(sockfd);
    exit(EXIT_SUCCESS);
}
