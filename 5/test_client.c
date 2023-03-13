#include "mysocket.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int sockfd;
    struct sockaddr_in serv_addr;

    char buff[5000];
    memset(buff, 0, 50);
    if((sockfd = my_socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(my_connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");
    int recv_len=my_recv(sockfd, buff, 5000, 0);
    printf("Received %d bytes\n", recv_len);
    for(int i = 0; i < recv_len; i++){
        printf("%c", buff[i]);
    }
    memset(buff, 0, 5000);
    printf("\n");
    recv_len=my_recv(sockfd, buff, 5000, 0);
    for(int i = 0; i < recv_len; i++){
        printf("%c", buff[i]);
    }
    memset(buff, 0, 5000);
    printf("\n");
    recv_len=my_recv(sockfd, buff, 5000, 0);
    for(int i = 0; i < recv_len; i++){
        printf("%c", buff[i]);
    }
    memset(buff, 0, 5000);
    printf("\n");
    // recv_len=my_recv(sockfd, buff, 50, 0);
    // for(int i = 0; i < 50; i++){
    //     printf("%c", buff[i]);
    // }
    // printf("\n");

    strcpy(buff, "Hello from client");
    my_send(sockfd, buff, 50, 0);

    strcpy(buff, "Hello from client 2");
    my_send(sockfd, buff, 50, 0);

    strcpy(buff, "Hello from client 3");
    my_send(sockfd, buff, 50, 0);
    
    my_close(sockfd);

    return 0;
}