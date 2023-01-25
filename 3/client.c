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
#define BUFF_MAX 100

        /* THE CLIENT PROCESS */

// recv function to receive data from server
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
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	char buffer[BUFF_MAX];

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(atoi(argv[1]));

	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	memset(buffer, 0, BUFF_MAX);
	int n = myrecv(sockfd, buffer);

	if(n==0)
		printf("Server closed connection\n");
	else 
		printf("%s\n", buffer);
		
	close(sockfd);

	return 0;
}

