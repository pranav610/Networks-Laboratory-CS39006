/*
			NETWORK PROGRAMMING WITH SOCKETS
        Assignment 1 (TCP Client-Server Programming)
    B. A simple TCP iterative server and client to evaluate arithmetic expressions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define buffsize 20

            /* THE CLIENT PROCESS */

int main(int argc, char* argv[])
{
    int sockfd; /* Socket descriptor */
    struct sockaddr_in serv_addr; /* Server address */

    char buffer[buffsize];
    int lenrem;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
		perror("Unable to create socket\n");
		exit(0);
	}

    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(20000);

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Unable to connect to server\n");
        exit(0);
    }

    while (1)
    {
        // Request the user to provide an expression for evaluation.
        printf("Enter an expression: \n");

        // Input fetching using getline
        int lenrem;
        char *in = NULL;
        size_t insize = 0;
        lenrem = getline(&in, &insize, stdin); 
        if(strlen(in)==3)
            if(in[0]=='-' && in[1]=='1' && in[2]=='\n')
                break;
        
        // remove the newline character
        in[lenrem-1] = '\0';

        // send the input to server
        while(lenrem>=buffsize)
        {   
            char *temp;
            printf("HERE1\n");
            temp = (char *)malloc(buffsize * sizeof(char));
            printf("HERE1\n");
            strncpy(temp, in, buffsize-1);
            temp[buffsize-1] = '\0';
            printf("%s\n", temp);
            printf("HERE1\n");
            send(sockfd, temp, buffsize, 0);
            printf("HERE1\n");
            lenrem = lenrem - buffsize + 1;
            in = in + buffsize - 1;
        }
        send(sockfd, in, lenrem, 0);

        // recieve the result from server
        recv(sockfd, buffer, buffsize, 0);

        // print the result
        if(strcmp(buffer, "Invalid Expression") == 0)
            printf("Given expression is invalid\n\n");
        else
            printf("Given expression evaluates to: %s\n\n", buffer);
        
    }

    printf("Exiting...\n");

    return 0;
}