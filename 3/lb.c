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
#include <poll.h>
#include <time.h>
#define BUFF_MAX 100
#define TIMEOUT 5000

        /* THE LOAD BALANCER PROCESS */

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
    int sockfd, newsockfd, sockfds1, sockfds2 ; // Socket descriptors
    socklen_t clilen; // client length
    struct sockaddr_in cli_addr, serv_addr, s1_addr, s2_addr;
    char buffer[BUFF_MAX];
    memset(buffer, 0, BUFF_MAX);
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
    memset(&s1_addr, 0, sizeof(s1_addr));
    memset(&s2_addr, 0, sizeof(s2_addr));

    // create socket for load balancer that will listen to client
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket\n");
        exit(EXIT_FAILURE);
    }

    // set up server address for load balancer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    // set up server address for server 1
    s1_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    s1_addr.sin_port = htons(atoi(argv[2]));

    // set up server address for server 2
    s2_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    s2_addr.sin_port = htons(atoi(argv[3]));

    // bind load balancer socket to address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Cannot bind\n");
        exit(EXIT_FAILURE);
    }

    nfds_t sz = 1;
    struct pollfd fdset[1];
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;

    int ret;
    int load1, load2;

    // setup load balancer to listen to client
    listen(sockfd, 5);
    clock_t start = clock();
    double dur = 0.0;
    int T;

    while(1){
        if(dur < TIMEOUT && dur > 0)
            T = TIMEOUT - dur;
        else 
            T = TIMEOUT;
        start = clock();
        ret = poll(fdset, sz, T);

        if(ret < 0)
        {
            perror("Poll failed\n");
            close(sockfd);
            close(sockfds1);
            close(sockfds2);
            exit(EXIT_FAILURE);
        }

        // ret = 0 means timeout
        if(ret == 0)
        {   
            // create socket for server 1
            if ((sockfds1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("Cannot create socket\n");
                exit(EXIT_FAILURE);
            }

            // create socket for server 2
            if ((sockfds2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("Cannot create socket\n");
                exit(EXIT_FAILURE);
            }

            // update load value of server 1
            memset(buffer, 0, BUFF_MAX);
            sprintf(buffer, "Send Load");
            // connect to server1
            if (connect(sockfds1, (struct sockaddr *) &s1_addr, sizeof(s1_addr)) < 0) {
                perror("Cannot connect\n");
                close(sockfd);
                close(sockfds1);
                close(sockfds2);
                exit(EXIT_FAILURE);
            }
            send(sockfds1, buffer, strlen(buffer) + 1, 0);
            memset(buffer, 0, BUFF_MAX);
            int n = myrecv(sockfds1, buffer);
            if(n==0){
                perror("Server-1 Disconnected\n");
                close(sockfd);
                close(sockfds1);
                close(sockfds2);
                exit(EXIT_FAILURE);
            }
            load1 = atoi(buffer);
            printf("Load received from %s %d\n", inet_ntoa(s1_addr.sin_addr), load1);
            close(sockfds1);

            // update load value of server 2
            memset(buffer, 0, BUFF_MAX);
            sprintf(buffer, "Send Load");
            // connect to server1
            if (connect(sockfds2, (struct sockaddr *) &s2_addr, sizeof(s2_addr)) < 0) {
                perror("Cannot connect\n");
                close(sockfd);
                close(sockfds1);
                close(sockfds2);
                exit(EXIT_FAILURE);
            }
            send(sockfds2, buffer, strlen(buffer) + 1, 0);
            memset(buffer, 0, BUFF_MAX);
            n = myrecv(sockfds2, buffer);
            if(n==0){
                perror("Server-1 Disconnected\n");
                close(sockfd);
                close(sockfds1);
                close(sockfds2);
                exit(EXIT_FAILURE);
            }
            load2 = atoi(buffer);
            printf("Load received from %s %d\n", inet_ntoa(s2_addr.sin_addr), load2);
            close(sockfds2);
            dur = 0;
        }

        // ret > 0 means there is a request from client
        if(ret > 0){
            dur = (clock() - start) / (double) CLOCKS_PER_SEC;
            dur *= 1000;
            if(fdset[0].revents == POLLIN){
                clilen = sizeof(cli_addr);
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0) {
                    perror("Cannot accept\n");
                    close(sockfd);
                    close(sockfds1);
                    close(sockfds2);
                    close(newsockfd);
                    exit(EXIT_FAILURE);
                }
                if(fork()==0){
                    close(sockfd);
                    memset(buffer, 0, BUFF_MAX);
                    if(load1 < load2){
                        // create socket for server 1
                        if ((sockfds1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                            perror("Cannot create socket\n");
                            exit(EXIT_FAILURE);
                        }
                        // request to server 1 for time
                        memset(buffer, 0, BUFF_MAX);
                        sprintf(buffer, "Send Time");
                        // connect to server1
                        if (connect(sockfds1, (struct sockaddr *) &s1_addr, sizeof(s1_addr)) < 0) {
                            perror("Cannot connect\n");
                            close(newsockfd);
                            close(sockfds1);
                            close(sockfds2);
                            exit(EXIT_FAILURE);
                        }

                        send(sockfds1, buffer, strlen(buffer) + 1, 0);
                        memset(buffer, 0, BUFF_MAX);
                        int n = myrecv(sockfds1, buffer);
                        if(n==0){
                            perror("Server-1 Disconnected\n");
                            close(newsockfd);
                            close(sockfds1);
                            close(sockfds2);
                            exit(EXIT_FAILURE);
                        }
                        printf("Sending client request to %s\n", inet_ntoa(cli_addr.sin_addr));
                        send(newsockfd, buffer, strlen(buffer) + 1, 0);
                        close(sockfds1);
                    }else{
                        // create socket for server 2
                        if ((sockfds2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                            perror("Cannot create socket\n");
                            exit(EXIT_FAILURE);
                        }
                        // request to server 2 for time
                        memset(buffer, 0, BUFF_MAX);
                        sprintf(buffer, "Send Time");
                        // connect to server2
                        if (connect(sockfds2, (struct sockaddr *) &s2_addr, sizeof(s2_addr)) < 0) {
                            perror("Cannot connect\n");
                            close(sockfds1);
                            close(sockfds2);
                            close(newsockfd);
                            exit(EXIT_FAILURE);
                        }

                        send(sockfds2, buffer, strlen(buffer) + 1, 0);
                        memset(buffer, 0, BUFF_MAX);
                        int n = myrecv(sockfds2, buffer);
                        if(n==0){
                            perror("Server-2 Disconnected\n");
                            close(newsockfd);
                            close(sockfds1);
                            close(sockfds2);
                            exit(EXIT_FAILURE);
                        }
                        printf("Sending client request to %s\n", inet_ntoa(cli_addr.sin_addr));
                        send(newsockfd, buffer, strlen(buffer) + 1, 0);
                        close(sockfds2);
                    }
                    close(newsockfd);
                    exit(EXIT_SUCCESS);
                }
                close(newsockfd);
            }
        }
    }
}