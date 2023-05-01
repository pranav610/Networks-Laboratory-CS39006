#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){
    // char buffer[256];
    // int sockfd, newsockfd;
    // struct sockaddr_in serv_addr, cli_addr;

    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd < 0) {
    //     perror("ERROR opening socket");
    //     exit(1);
    // }
    // int n = recv(sockfd, buffer, 255, 0);
    // printf("%d\n", n);

    // bzero((char *) &serv_addr, sizeof(serv_addr));
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    // serv_addr.sin_port = htons(5000);

    // if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    //     perror("ERROR on binding");
    //     exit(1);
    // }

    // listen(sockfd, 5);


    // n = recv(sockfd, buffer, 255, 0);
    // printf("%d\n", n);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
}