// HTTP1.1 Server to handle GET and PUT requests
// GET: returns the file requested
// PUT: creates a file with the name and content specified in the request
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFF_MAX 1024

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
    char buffer[BUFF_MAX];

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    // Set the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }

    // Listen for connections
    listen(sockfd, 5);

    // Accept a connection
    while (1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }

        // fork a child process to handle the request
        if (fork() == 0)
        {
            close(sockfd);
            // create a string which will realloc as we receive data
            char *msg = (char *)malloc(BUFF_MAX);
            memset(msg, 0, BUFF_MAX);
            memset(buffer, 0, BUFF_MAX);
            int n = recv(newsockfd, buffer, BUFF_MAX, 0);
            int newsize, oldsize;

            while (buffer[n - 1] != '\0')
            {
                oldsize = strlen(msg) + 1;
                newsize = oldsize + n;

                msg = (char *)realloc(msg, newsize);
                strncat(msg, buffer, n);

                memset(buffer, 0, sizeof(buffer));
                n = recv(sockfd, buffer, BUFF_MAX, 0);
                if (n == 0)
                    break;
            }
            if (n == 0)
            {
                printf("Client closed connection\n");
                exit(EXIT_SUCCESS);
            }
            oldsize = strlen(msg) + 1;
            newsize = oldsize + strlen(buffer);
            msg = (char *)realloc(msg, newsize);
            strcat(msg, buffer);

            printf("Message received: %s\n", msg);

            // Parse the request
            // get method, path and version
            char *method = strtok(msg, " ");
            char *path = strtok(NULL, " ");
            path++;
            char *version = strtok(NULL, "\r\n");
            if (strcmp(method, "GET") == 0)
            {
                // open the file
                printf("Here\n");
                printf("Path: %s\n", path);
                FILE *fp = fopen(path, "r");
                if (fp == NULL)
                {
                    // file not found

                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "%s 404 Not Found\r\n", version);
                    send(newsockfd, buffer, strlen(buffer) + 1, 0);
                    close(newsockfd);
                    exit(EXIT_SUCCESS);
                }
                else
                {
                    // close the file
                    fclose(fp);
                    /* get values of the following headers:
                    1.host,
                    2.connection,
                    3.accept,
                    4.accept-language,
                    5.If-Modified-Since
                    */
                    printf("Here2\n");
                    char *values[5];
                    // parse through headers to get values of the above headers
                    while (1)
                    {
                        char *header = strtok(NULL, "\r\n");
                        if (header == NULL)
                        {
                            break;
                        }
                        // get key and value without using strtok
                        char *key = header;
                        char *value = header;
                        while (*value != ':')
                        {
                            value++;
                        }
                        *value = '\0';
                        value++;

                        // remove leading spaces
                        while (value[0] == ' ' || value[0] == '\t')
                        {
                            value++;
                        }
                        if (strcmp(key, "Host") == 0)
                        {
                            values[0] = value;
                        }
                        else if (strcmp(key, "Connection") == 0)
                        {
                            values[1] = value;
                        }
                        else if (strcmp(key, "Accept") == 0)
                        {
                            values[2] = value;
                        }
                        else if (strcmp(key, "Accept-Language") == 0)
                        {
                            values[3] = value;
                        }
                        else if (strcmp(key, "If-Modified-Since") == 0)
                        {
                            values[4] = value;
                        }
                    }
                    printf("Here3\n");
                    for(int i=0;i<5;i++)
                    {
                        printf("%s\n",values[i]);
                    }
                    // send the file only if the file has been modified since the date specified in the request
                    if (values[4] != NULL)
                    {
                        struct tm tm;
                        strptime(values[4], "%a %d %b %Y %H:%M:%S %Z", &tm);
                        // printf("Time: %s\n", values[4]);
                        time_t header_time = timegm(&tm);

                        struct stat file_stat;
                        if (stat(path, &file_stat) == 0)
                        {   
                            // printf("File modified time: %s\n", ctime(&file_stat.st_mtime));
                            if (difftime(header_time, file_stat.st_mtime) >= 0)
                            {
                                // The file has not been modified since the specified time
                                // send a "Not Modified" response
                                memset(buffer, 0, BUFF_MAX);
                                sprintf(buffer, "%s 304 Not Modified\r\n", version);
                                send(newsockfd, buffer, strlen(buffer) + 1, 0);
                                close(newsockfd);
                                exit(EXIT_SUCCESS);
                            }
                        }
                        else
                        {
                            perror("stat");
                            exit(EXIT_FAILURE);
                        }
                    }

                    // The file has been modified since the specified time
                    // send the file contents
                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "%s 200 OK\r\n", version);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    // create headers for the response
                    // 1. Expires: always set to current time + 3 days
                    time_t now = time(NULL);
                    struct tm *now_tm = gmtime(&now);
                    now_tm->tm_mday += 3;
                    mktime(now_tm);
                    char expires[100];
                    strftime(expires, 100, "%a, %d %b %Y %H:%M:%S %Z", now_tm);
                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "Expires: %s\r\n", expires);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    // 2. Cache-control: should set to no-store always
                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "Cache-Control: no-store\r\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    // 3. Content-language: set to en-us
                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "Content-Language: en-us\r\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    // 4. Content-length
                    struct stat st;
                    stat(path, &st);
                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "Content-Length: %ld\r\n", st.st_size);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    // 5. Content-type: set to text/html if the file asked in the url has extension .html
                    // application/pdf if the file asked in the url has extension .pdf
                    // image/jpeg if the file asked for has extension .jpg
                    // text/* for anything else
                    char content_type[100];
                    char *ext = strrchr(path, '.');
                    if (strcmp(ext, ".html") == 0)
                    {   
                        strcpy(content_type, "text/html");
                    }
                    else if (strcmp(ext, ".pdf") == 0)
                    {   
                        strcpy(content_type, "application/pdf");
                    }
                    else if (strcmp(ext, ".jpg") == 0)
                    {   
                        strcpy(content_type, "image/jpeg");
                    }
                    else
                    {
                        strcpy(content_type, "text/*");
                    }
                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "Content-Type: %s\r\n", content_type);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    // put an empty line to indicate the end of the headers and start sending the file
                    memset(buffer, 0, BUFF_MAX);
                    sprintf(buffer, "\r\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    // send the file
                    FILE *fp = fopen(path, "r");
                    size_t nread;
                    memset(buffer, 0, BUFF_MAX);
                    // printf("Here4\n");
                    while ((nread = fread(buffer, 1, BUFF_MAX, fp)) > 0)
                    {
                        send(newsockfd, buffer, nread, 0);
                        // print n characters sent
                        // printf("%.*s", (int)nread, buffer);
                        memset(buffer, 0, BUFF_MAX);
                    }
                    fclose(fp);
                    // send null byte to indicate end of file
                    // send(newsockfd, "\r\n", 2, 0);
                    // close the connection
                    close(newsockfd);
                    exit(EXIT_SUCCESS);
                }
            }
            // else if (strcmp(method, "PUT") == 0)
            // {
            //     // open the file
            //     FILE *fp = fopen(path, "r");
            //     if (fp == NULL)
            //     {
            //         // file not found
            //         memset(buffer, 0, BUFF_MAX);
            //         sprintf(buffer, "%s 404 Not Found\r\n", version);
            //         send(newsockfd, buffer, strlen(buffer) + 1, 0);
            //     }
            //     else
            //     {
                    
            //     }
            // }
            else
            {
                // method not supported
                char *response = "HTTP/1.1 501 Not Implemented\r\n";
                send(newsockfd, response, strlen(response)+1, 0);
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            close(newsockfd);
        }
    }
}