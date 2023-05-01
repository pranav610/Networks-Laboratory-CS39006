/*
			NETWORK PROGRAMMING WITH SOCKETS
        Assignment 1 (TCP Client-Server Programming)
    A. Write a TCP iterative server to allow client programs to get the system date and time from the server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

            /* THE CLIENT PROCESS */

int main(int argc, char *argv[])
{
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	char buffer[256];

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}

	/* Recall that we specified INADDR_ANY when we specified the server
	   address in the server. Since the client can run on a different
	   machine, we must specify the IP address of the server. 

	   In this program, we assume that the server is running on the
	   same machine as the client. 127.0.0.1 is a special address
	   for "localhost" (this machine)

	
    */
    serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(atoi(argv[1]));

	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	/* After connection, the client can send or receive messages.
	   However, please note that recv() will block when the
	   server is not sending and vice versa. Similarly send() will
	   block when the server is not receiving and vice versa. For
	   non-blocking modes, refer to the online man pages.
	*/

	bzero(buffer,256);
    sleep(20);
	recv(sockfd, buffer, 256, 0);
	printf("%s\n", buffer);

    send(sockfd, "Hello", 6, 0);
	
    sleep(5);
	close(sockfd);
	return 0;

}

