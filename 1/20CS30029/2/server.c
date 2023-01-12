/*
            NETWORK PROGRAMMING WITH SOCKETS
        Assignment 1 (TCP Client-Server Programming)
    B. A simple TCP iterative server and client to evaluate arithmetic expressions.
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
#include <math.h>
#define buffsize 20

/* THE SERVER PROCESS */

int isOp(char a)
{
    if(a=='+' || a=='-' || a=='*' || a=='/')
        return 1;
    return 0;
}

int isDigit(char a)
{
    if(a>='0' && a<='9')
        return 1;
    return 0;
}

// Function to evaluate the expression
double evaluate(char *in)
{   
    printf("Expression received: %s\n", in);
    int exprsize = strlen(in);

    // white space removal 
    int fill = 0;
    for (int j = 0; j < exprsize; j++)
    {   
        if (in[j] == ' ')
            continue;
        // if(!isDigit(in[j]) && !isOp(in[j]) && in[j]!='(' && in[j]!=')' && in[j]!='.')
        //     return INFINITY;
        in[fill++] = in[j];
    }
    printf("Expression after white space removal: %s\n", in);
    in[fill] = '\0';
    exprsize = fill;

    double ans = 0;
    int itr = 0;
    int start = 0;
    // char neg = 0;
    char glbop = '+';
    // char invalid = 0;

    // if (isOp(in[itr]))
    //     return INFINITY;

    for (; itr < exprsize; itr++)
    {
        if (isOp(in[itr]))
        {
            char op;
            double num;

            // if (start == itr)
            //     return INFINITY;
            
            if(in[start]=='(')
            {
                for(itr = start+1; itr<exprsize; itr++) if(in[itr]==')') break;
                // if(itr==exprsize) return INFINITY;
                in[itr] = '\0';
                num = evaluate(in+1+start);
                in[itr] = ')';
                op = in[++itr];
            }
            else
            {   
                op = in[itr];
                in[itr] = '\0';
                num = atof(in + start);
                in[itr] = op;
            }
            
            if (glbop == '+')
                ans += num;
            else
            {
                if (glbop == '-')
                    ans -= num;
                else
                {
                    if (glbop == '*')
                        ans *= num;
                    else
                    {
                        if (glbop == '/')
                        {
                            ans /= num;
                        }
                    }
                }
            }
            glbop = op;
            start = itr + 1;
        }
    }

    if (itr == exprsize)
    {
        // if (start == itr)
        //     return INFINITY;
        
        double num;

        if(in[start]=='(')
        {
            for(itr = start + 1; itr<exprsize; itr++) if(in[itr]==')') break;
            // if(itr==exprsize) return INFINITY;
            in[itr] = '\0';
            num = evaluate(in+1+start);
            in[itr] = ')';
        }
        else
            num = atof(in + start);

        if (glbop == '+')
            ans += num;
        else
        {
            if (glbop == '-')
                ans -= num;
            else
            {
                if (glbop == '*')
                    ans *= num;
                else
                {
                    if (glbop == '/')
                    {
                        // if (num == 0)
                        //     return INFINITY;
                        // else
                            ans /= num;
                    }
                }
            }
        }
    }
    
    return ans;
}

int main()
{
    int sockfd, newsockfd;                  // Socket descriptors
    struct sockaddr_in serv_addr, cli_addr; // Server and client addresses
    int clilen = sizeof(cli_addr);          // size of struct client
    char buffer[buffsize];                  // Buffer to store message

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(20000);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    printf("Server is running...\n");

    listen(sockfd, 10);

    while (1)
    {   
        // Accept a connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("Accept Error\n");
            exit(0);
        }
        printf("Client Connected\n");

        char *expr;
        expr = (char *)malloc(buffsize * sizeof(char));

        // now as connection is established with the client we can start reading
        // message sent by the client

        while (1)
        {
            int ret = recv(newsockfd, buffer, buffsize, 0);

            if (ret == 0)
            {
                printf("Client Disconnected\n");
                break;
            }

            int oldsize = strlen(expr) + 1;
            int newsize = oldsize + strlen(buffer);

            printf("oldsize: %d, newsize: %d\n", oldsize, newsize);

            if (newsize != oldsize)
            {
                expr = (char *)realloc(expr, newsize * sizeof(char));
                char *ptr = expr + oldsize - 1;
                strcpy(ptr, buffer);
            }

            // data is sent from client without new line at the end
            if (strlen(buffer) < buffsize - 1)
            {
                double ans = evaluate(expr);
                // if (ans == INFINITY)
                // {
                //     strcpy(buffer, "Invalid Expression");
                //     send(newsockfd, buffer, strlen(buffer) + 1, 0);
                // }
                // else
                // {
                    gcvt(ans, 10, buffer);
                    send(newsockfd, buffer, strlen(buffer) + 1, 0);
                // }
                expr = (char *)realloc(expr, buffsize * sizeof(char));
                bzero(expr, buffsize);
                expr[0] = '\0';
            }
        }
    }
}