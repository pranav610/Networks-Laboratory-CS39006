#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    int a = 5;
    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    send(sockfd, &a, sizeof(a), 0);
    close(sockfd);
    return 0;
}