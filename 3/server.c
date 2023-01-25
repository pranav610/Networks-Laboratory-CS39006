/*
        NETWORK PROGRAMMING WITH SOCKETS
        Assignment 3 (Load Balancer)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>  
#define BUFF_MAX 100

		/* THE SERVER PROCESS */

// function to create random integer between 1 and 100
int load_generator()
{
	int load = rand() % 100 + 1;
	return load;
}

// recv function to receive data from client
int myrecv(int sockfd, char *buff)
{   
	int ret = 0;
    char temp[BUFF_MAX];
    memset(temp, 0, sizeof(temp));

    int n = recv(sockfd, temp, BUFF_MAX, 0);
	ret += n;
    if(n==0)
		return ret;
    strncpy(buff, temp, n);
    while(temp[n-1]!='\0')
    {
        memset(temp, 0, sizeof(temp));
        n = recv(sockfd, temp, BUFF_MAX, 0);
		ret+=n;
		strncat(buff, temp, n);
    }
	return ret;
}

int main(int argc, char *argv[])
{	
	srand(time(0));	// seed random number generator
	int			sockfd, newsockfd ; // Socket descriptors
	socklen_t	clilen; // client length
	struct sockaddr_in	cli_addr, serv_addr;
    char buffer[BUFF_MAX];
	memset(buffer, 0, BUFF_MAX);
	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(&cli_addr, 0, sizeof(cli_addr));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(EXIT_FAILURE);
	}

	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		    = htons(atoi(argv[1]));

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(EXIT_FAILURE);
	}

	// listen for connections
	listen(sockfd, 10); 

	while (1) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen) ;

		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(EXIT_FAILURE);
		}

		while(1)
		{
			memset(buffer, 0, BUFF_MAX);
			int n = myrecv(newsockfd, buffer);
			if(n==0){
				printf("Client disconnected\n");
				break;
			}
			if(strcmp(buffer, "Send Load") == 0){
				int load = load_generator();
				memset(buffer, 0, BUFF_MAX);
				sprintf(buffer, "%d", load);
				send(newsockfd, buffer, strlen(buffer) + 1, 0);
				printf("Load sent: %d\n", load);
			}else if(strcmp(buffer, "Send Time") == 0){
				time_t rawtime;
				struct tm * timeinfo;
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				sprintf(buffer, "Current local time and date: %s", asctime (timeinfo));
				send(newsockfd, buffer, strlen(buffer) + 1, 0);
			}else
				printf("Invalid response from the load balancer\n");
		}
		close(newsockfd);
	}
	return 0;
}
			

