#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    if(bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))<0){
        perror("Bind failed");
        exit(1);
    }

    listen(sockfd, 5);
    int newSocket = accept(sockfd, (struct sockaddr *) &clientAddr, &clientAddrSize);
    int a, b;
    int n = recv(newSocket, &a, 1, 0);
    b >>= n;
    b += a;
    n = recv(newSocket, &a, 2, 0);
    b >>= n;
    b += a;
    n = recv(newSocket, &a, 1, 0);
    b >>= n;
    b += a;


    printf("%d\n", b);
    close(newSocket);
    close(sockfd);
    return 0;
    
}