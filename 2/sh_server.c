/*
			NETWORK PROGRAMMING WITH SOCKETS
        Assignment 2 (TCP and UDP Client-Server Programming)
    B. Client Server Shell interaction
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#define BUFF_MAX 50
#define USER_MAX 25
#define FILE_NAME "users.txt"


            /* The Server Process*/
int isValid(char *username)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(FILE_NAME, "r");
    if (fp == NULL)
        return 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        line[read-1] = '\0';
        if(strcmp(username, line)==0)
            return 1;
    }
    fclose(fp);
    if (line)
        free(line);
    return 0;
    
}

int main()
{
    int sockfd, newsockfd;                  // Socket descriptors
    struct sockaddr_in servaddr, cliaddr;   // Server and client addresses
    socklen_t clilen = sizeof(cliaddr);           // size of struct client
    char buffer[BUFF_MAX];                  // Buffer to store message
    memset(buffer, 0, sizeof(buffer));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(20000);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    printf("Server is running...\n");

    listen(sockfd, 10);

    while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        
        if(fork()==0)
        {
            close(sockfd);
            if (newsockfd < 0)
            {
                perror("Accept Error\n");
                exit(EXIT_FAILURE);
            }
            printf("Client Connected\n");
            send(newsockfd, "LOGIN:", 7, 0);

            char username[USER_MAX];
            recv(newsockfd, username, USER_MAX, 0);
            printf("Username: %s\n", username);

            if(isValid(username))
                {send(newsockfd, "FOUND", 6, 0); printf("FOUND\n");}
            else
            {   
                printf("NOT-FOUND\n");
                printf("Client Disconnected\n");
                send(newsockfd, "NOT-FOUND", 10, 0);
                close(newsockfd);
                exit(EXIT_SUCCESS);
            }

            printf("HERE1\n");
                
            char *cmd;
            cmd = (char *)malloc(BUFF_MAX * sizeof(char));
            memset(cmd, 0, sizeof(cmd));

            // now as connection is established with the client we can start reading
            // message sent by the client

            while (1)
            {   

                memset(buffer, 0, sizeof(buffer));
                int ret = recv(newsockfd, buffer, BUFF_MAX, 0);

                printf("raw command: %s, lenght read: %d\n", buffer, ret);

                if (ret == 0)
                    break;

                if(buffer[ret-1]!='\0')
                    printf("Kuch toh gadbad hai\n");
                else 
                    printf("Sab sahi hai\n");
                
                printf("WTF!!\n");
                
                printf("strlen: %zu\n", strlen(cmd));

                int newsize, oldsize = strlen(cmd) + 1;

                if (buffer[ret - 1] != '\0')
                    newsize = oldsize + ret;
                else
                    newsize = oldsize + strlen(buffer);

                printf("%d=new, %d=old\n", newsize, oldsize);

                if (newsize != oldsize)
                {
                    cmd = (char *)realloc(cmd, newsize * sizeof(char));
                    strncat(cmd, buffer, ret);
                }

                // NULL character will denote end of the input
                if (buffer[ret - 1] == '\0')
                {
                    // send command
                    printf("Command recieved: %s\n",cmd);
                    int cmdsz = strlen(cmd);
                    int itr = 0;
                    for(; itr<cmdsz; itr++)
                        if(cmd[itr]!=' ') break;
                    char cmdd[4];
                    int count = 0;
                    memset(cmdd, 0, sizeof(cmdd));
                    for(; itr<cmdsz; itr++) 
                    {
                        if(cmd[itr]==' ') break;
                        cmdd[count++] = cmd[itr];
                        if(count==4)
                        {
                            send(newsockfd, "$$$$", 5, 0);
                            cmd = (char *) realloc(cmd, BUFF_MAX*sizeof(char));
                            memset(cmd, 0, sizeof(cmd));
                            continue;
                        }
                    }
                    printf("1st 4 characters of command: %s\n", cmdd);

                    if(strcmp(cmdd, "cd")==0)
                    {   
                        for(; itr<cmdsz; itr++)
                            if(cmd[itr]!=' ') break;
                        printf("cd\n");
                        printf("%s\n", cmd+itr);
                        int val = chdir(cmd+itr);
                        if(val < 0)
                        {
                            send(newsockfd, "####", 5, 0);
                            cmd = (char *) realloc(cmd, BUFF_MAX*sizeof(char));
                            memset(cmd, 0, sizeof(cmd));
                            continue;
                        }
                        send(newsockfd, "", 1, 0);
                    }
                        
                    if(strcmp(cmdd, "dir")==0)
                    {   
                        for(; itr<cmdsz; itr++)
                            if(cmd[itr]!=' ') break;
                        printf("dir\n");
                        printf("%s\n", cmd+itr);
                        DIR *dir = opendir(cmd+itr);
                        if(!dir)
                        {
                            send(newsockfd, "####", 5, 0);
                            cmd = (char *) realloc(cmd, BUFF_MAX*sizeof(char));
                            memset(cmd, 0, sizeof(cmd));
                            continue;
                        }
                        struct dirent * workingdir = readdir(dir);
                        while(workingdir)
                        {
                            int count = 0, itr = 0;
                            memset(buffer, 0 , sizeof(buffer));
                            printf("%s\n", workingdir->d_name);
                            for(; itr<256; itr++)
                            {   
                                if(workingdir->d_name[itr]=='\0') break;
                                buffer[count++] = workingdir->d_name[itr];
                                if(count==BUFF_MAX)
                                {
                                    send(newsockfd, buffer, BUFF_MAX, 0);
                                    count = 0;
                                    memset(buffer, 0, sizeof(char));
                                }
                            }
                            buffer[count++] = '\0';
                            send(newsockfd, buffer, count, 0);
                            workingdir = readdir(dir);
                        }
                        closedir(dir);
                    }
                    if(strcmp(cmdd, "pwd")==0)
                    {   
                        printf("pwd\n");
                        char pwd[256];
                        if (getcwd(pwd, sizeof(pwd)) == NULL)
                        {
                            send(newsockfd, "####", 5, 0);
                            cmd = (char *) realloc(cmd, BUFF_MAX*sizeof(char));
                            memset(cmd, 0, sizeof(cmd));
                            continue;
                        }
                        printf("%s\n", pwd);
                        int count = 0, itr = 0;
                        memset(buffer, 0 , sizeof(buffer));
                        for(; itr<256; itr++)
                        {   
                            if(pwd[itr]=='\0') break;
                            buffer[count++] = pwd[itr];
                            if(count==BUFF_MAX)
                            {
                                send(newsockfd, buffer, BUFF_MAX, 0);
                                count = 0;
                                memset(buffer, 0, sizeof(char));
                            }
                        }
                        buffer[count++] = '\0';
                        send(newsockfd, buffer, count, 0);
                    }
                    cmd = (char *) realloc(cmd, BUFF_MAX*sizeof(char));
                    memset(cmd, 0, sizeof(cmd));
                }
            }
            printf("Client Disconnected\n");
            close(newsockfd);
            exit(EXIT_SUCCESS);
        }
        close(newsockfd);
    }
}