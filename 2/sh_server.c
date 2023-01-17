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
#define USER_MAX 26
#define FILE_NAME "users.txt"

            /* The Server Process*/

// function to check if username is valid
int isValid(char *username)
{   
    // open file and check if username is present
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(FILE_NAME, "r");
    if (fp == NULL)
        return 0;

    // read line by line and check if username is present
    while ((read = getline(&line, &len, fp)) != -1)
    {
        line[read - 1] = '\0';
        if (strcmp(username, line) == 0)
            return 1;
    }

    // close file and free memory
    fclose(fp);
    if (line)
        free(line);
    return 0;
}

int main()
{
    int sockfd, newsockfd;                // Socket descriptors
    struct sockaddr_in servaddr, cliaddr; // Server and client addresses
    socklen_t clilen = sizeof(cliaddr);   // size of struct client
    char buffer[BUFF_MAX];                // Buffer to store message
    memset(buffer, 0, sizeof(buffer));

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    // Bind the socket to an address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(20000);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    printf("Server is running...\n");

    // set up queue for incoming connections
    listen(sockfd, 10);

    while (1)
    {   
        // accept incoming connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);

        // fork a child process to handle the connection 
        if (fork() == 0)
        {   
            // close the listening socket
            close(sockfd);
            // check if connection was successful
            if (newsockfd < 0)
            {
                perror("Accept Error\n");
                exit(EXIT_FAILURE);
            }
            printf("Client Connected\n");
            // send message to client to login
            send(newsockfd, "LOGIN:", 7, 0);

            // receive username from client
            char username[USER_MAX];
            recv(newsockfd, username, USER_MAX, 0);

            // check if username is valid
            if (isValid(username))
            {   
                printf("%s logged in\n", username);
                send(newsockfd, "FOUND", 6, 0);
            }
            else
            {   
                // if username is invalid, close connection
                printf("Client Disconnected\n");
                send(newsockfd, "NOT-FOUND", 10, 0);
                close(newsockfd);
                exit(EXIT_SUCCESS);
            }

            // cmd will realloc and store complete incoming message
            char *cmd;
            cmd = (char *)malloc(BUFF_MAX * sizeof(char));
            memset(cmd, 0, BUFF_MAX);

            // now as connection is established with the client we can start reading
            // message sent by the client

            while (1)
            {
                memset(buffer, 0, sizeof(buffer));
                int ret = recv(newsockfd, buffer, BUFF_MAX, 0);

                if (ret == 0)
                    break;

                // calculate new size of cmd and realloc

                int newsize, oldsize = strlen(cmd) + 1;

                if (buffer[ret - 1] != '\0')
                    newsize = oldsize + ret;
                else
                    newsize = oldsize + strlen(buffer);

                if (newsize != oldsize)
                {
                    cmd = (char *)realloc(cmd, newsize * sizeof(char));
                    strncat(cmd, buffer, ret);
                }

                // NULL character will denote end of the input
                if (buffer[ret - 1] == '\0')
                {
                    int cmdsz = strlen(cmd);
                    // remove leading spaces
                    int itr = 0;
                    for (; itr < cmdsz; itr++)
                        if (cmd[itr] != ' ')
                            break;
                    // extract first 3 characters of the command
                    char cmdd[4];
                    int count = 0;
                    memset(cmdd, 0, sizeof(cmdd));
                    for (; itr < cmdsz; itr++)
                    {
                        if (cmd[itr] == ' ')
                            break;
                        cmdd[count++] = cmd[itr];
                        if (count == 4)
                            break;
                    }
                    // first continuos word with more than 3 characters will 
                    // denote a invalid command
                    if(count == 4)
                    {
                        send(newsockfd, "$$$$", 5, 0);
                        cmd = (char *)realloc(cmd, BUFF_MAX * sizeof(char));
                        memset(cmd, 0, BUFF_MAX);
                        continue;
                    }

                    if (strcmp(cmdd, "cd") == 0)
                    {   
                        // remove leading spaces
                        for (; itr < cmdsz; itr++)
                            if (cmd[itr] != ' ')
                                break;
                        // change directory
                        int val;
                        if(itr == cmdsz)
                        {
                            char home[50] = "/home/";
                            strcat(home, getlogin());
                            val = chdir(home);
                        }
                        else
                            val = chdir(cmd + itr);
                        if (val < 0)
                        {
                            send(newsockfd, "####", 5, 0);
                            cmd = (char *)realloc(cmd, BUFF_MAX * sizeof(char));
                            memset(cmd, 0, BUFF_MAX);
                            continue;
                        }
                        // send null character to client
                        send(newsockfd, "", 1, 0);
                    }
                    else if (strcmp(cmdd, "dir") == 0)
                    {   
                        // remove leading spaces
                        for (; itr < cmdsz; itr++)
                            if (cmd[itr] != ' ')
                                break;
                        DIR *dir;
                        if(itr == cmdsz)
                            dir = opendir(getcwd(NULL, 0));
                        else 
                            dir = opendir(cmd+itr);
                        if (!dir)
                        {
                            send(newsockfd, "####", 5, 0);
                            cmd = (char *)realloc(cmd, BUFF_MAX * sizeof(char));
                            memset(cmd, 0, BUFF_MAX);
                            continue;
                        }
                        // iterate through directory and send file names to client
                        struct dirent *workingdir = readdir(dir);
                        int count = 0;
                        memset(buffer, 0, sizeof(buffer));
                        while (workingdir)
                        {
                            for (int itr = 0; itr < 256; itr++)
                            {   
                                // add new line after each directory
                                if (workingdir->d_name[itr] == '\0')
                                    buffer[count++] = '\t';
                                else
                                    buffer[count++] = workingdir->d_name[itr];

                                if (count == BUFF_MAX)
                                {
                                    send(newsockfd, buffer, BUFF_MAX, 0);
                                    count = 0;
                                    memset(buffer, 0, sizeof(buffer));
                                }
                                if (workingdir->d_name[itr] == '\0')
                                    break;
                            }
                            // readdir will increment the pointer to next file
                            workingdir = readdir(dir);
                        }

                        buffer[count++] = '\0';
                        send(newsockfd, buffer, count, 0);
                        closedir(dir);
                    }
                    else if (strcmp(cmdd, "pwd") == 0)
                    {
                        char *pwd = getcwd(NULL, 0);
                        if (pwd == NULL)
                        {
                            send(newsockfd, "####", 5, 0);
                            cmd = (char *)realloc(cmd, BUFF_MAX * sizeof(char));
                            memset(cmd, 0, BUFF_MAX);
                            continue;
                        }
                        int count = 0, itr = 0;
                        memset(buffer, 0, sizeof(buffer));
                        for (; itr < strlen(pwd); itr++)
                        {
                            if (pwd[itr] == '\0')
                                break;
                            buffer[count++] = pwd[itr];
                            if (count == BUFF_MAX)
                            {
                                send(newsockfd, buffer, BUFF_MAX, 0);
                                count = 0;
                                memset(buffer, 0, sizeof(buffer));
                            }
                        }
                        buffer[count++] = '\0';
                        send(newsockfd, buffer, count, 0);
                    }
                    else
                        send(newsockfd, "$$$$", 5, 0);
                    cmd = (char *)realloc(cmd, BUFF_MAX * sizeof(char));
                    memset(cmd, 0, BUFF_MAX);
                }
            }
            printf("%s Disconnected\n", username);
            close(newsockfd);
            exit(EXIT_SUCCESS);
        }
        close(newsockfd);
    }
}