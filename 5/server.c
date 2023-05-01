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
#include <time.h>  

			/* THE SERVER PROCESS */

int main(int argc, char *argv[])
{
	int			sockfd, newsockfd ; /* Socket descriptors */
	socklen_t			clilen;
	struct sockaddr_in	cli_addr, serv_addr;
    char buffer[256];

	/* The following system call opens a socket. The first parameter
	   indicates the family of the protocol to be followed. For internet
	   protocols we use AF_INET. For TCP sockets the second parameter
	   is SOCK_STREAM. The third parameter is set to 0 for user
	   applications.
	*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

	/* The structure "sockaddr_in" is defined in <netinet/in.h> for the
	   internet family of protocols. This has three main fields. The
 	   field "sin_family" specifies the family and is therefore AF_INET
	   for the internet family. The field "sin_addr" specifies the
	   internet address of the server. This field is set to INADDR_ANY
	   for machines having a single IP address. The field "sin_port"
	   specifies the port number of the server.
	*/

	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		    = htons(atoi(argv[1]));

	/* With the information provided in serv_addr, we associate the server
	   with its port using the bind() system call. 
	*/

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5); /* This specifies that up to 5 concurrent client
			      requests will be queued up while the system is
			      executing the "accept" system call below.
			   */

	/* In this program we are illustrating an iterative server -- one
	   which handles client connections one by one.i.e., no concurrency.
	   The accept() system call returns a new socket descriptor
	   which is used for communication with the server. After the
	   communication is over, the process comes back to wait again on
	   the original socket descriptor.
	*/
	

		/* The accept() system call accepts a client connection.
		   It blocks the server until a client request comes.

		   The accept() system call fills up the client's details
		   in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note
		   that the new socket descriptor returned by the accept()
		   system call is stored in "newsockfd".
		*/
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
					&clilen) ;

		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}

        /*
            If the program reaches here this implies a client is connected to the server
            now this program will return server system date and time to the client    
        */

        
        bzero(buffer,256);
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        sprintf(buffer, "Current local time and date: %s", asctime (timeinfo));

        // send the message to the client
        send(newsockfd, buffer, strlen(buffer) + 1, 0);

        char mybuff[50];
        bzero(mybuff, 50);
        recv(newsockfd, mybuff, 50, 0);
        printf("Client: %s\n", mybuff);

        sleep(5);
		close(newsockfd);
        sleep(5);
        close(sockfd);
	
	return 0;
}
			

