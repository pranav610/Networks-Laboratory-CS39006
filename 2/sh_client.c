/*
			NETWORK PROGRAMMING WITH SOCKETS
        Assignment 2 (TCP and UDP Client-Server Programming)
    B. Client Server Shell interaction
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUFF_MAX 50
#define USER_MAX 26

            /*The Client Process*/

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    char buff[BUFF_MAX];
    memset(buff, 0, sizeof(buff));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Socket cretion failed\n");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    if(argc==2) inet_aton(argv[1], &servaddr.sin_addr);
    else inet_aton("127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(20000);

    if((connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
    {
        perror("Unable to connect to server\n");
        exit(EXIT_FAILURE);
    }

    recv(sockfd, buff, BUFF_MAX, 0);
    printf("%s ", buff);

    // char *username = (char *) malloc(USER_MAX * sizeof(char));
    // size_t len = USER_MAX;
    // int n = getline(&username, &len, stdin);
    // username[n-1] = '\0'; // replace newline read by getline with null character

    char username[USER_MAX];
    memset(username, 0, sizeof(username));
    int count = 0;
    char a = getchar();
    while (a!='\n')
    {
        username[count++] = a;
        a = getchar();
    }
    username[count++] = '\0';
    
    // following line will send username read by client to the server
    send(sockfd, username, count, 0);

    // after checking username against valid usernames the server's respond will be recieved 
    // using following command
    memset(buff, 0, sizeof(buff));
    recv(sockfd, buff, BUFF_MAX, 0);

    if(strcmp(buff, "NOT-FOUND")==0)
    {
        printf("Invalid username\n");
        close(sockfd);
        exit(EXIT_SUCCESS);
    }
    else
    {   
        printf("Login Successful\n");
        while(1)
        {   
            printf("%s:~$ ", username);
            memset(buff, 0, sizeof(buff));
            int count = 0, chunks = 0;
            char a = getchar();
            while (a!='\n')
            {
                buff[count++] = a;
                if(count==BUFF_MAX)
                {
                    send(sockfd, buff, BUFF_MAX, 0);
                    chunks++;
                    count = 0;
                    memset(buff, 0, sizeof(char));
                }
                a = getchar();
            } 
            buff[count++] = '\0';
            if(chunks==0 && strcmp(buff, "exit")==0) 
                break;
            send(sockfd, buff, count, 0);

            // recieve result from the server
            memset(buff, 0, sizeof(buff));
            int n = recv(sockfd, buff, BUFF_MAX, 0);
            if(buff[0]=='\0')
                continue;
            while(buff[n-1]!='\0')
            {
                printf("%.*s", BUFF_MAX, buff);
                memset(buff, 0, sizeof(buff));
                n = recv(sockfd, buff, BUFF_MAX, 0);
            }
            if(strcmp(buff, "$$$$")==0)
            {
                printf("Invalid command\n");
                continue;
            }
            if(strcmp(buff, "####")==0)
            {
                printf("Error in running command\n");
                continue;
            }
            printf("%s\n", buff);            
        }
    }    
}