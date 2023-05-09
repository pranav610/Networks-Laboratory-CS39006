# Networks-Laboratory-CS39006
This is the repository for the course Networks Laboratory CS39006 for the Spring semester 2022-23, at IIT Kharagpur.
## [Lab 1](https://github.com/pranav610/Networks-Laboratory-CS39006/tree/main/1)
TCP Iterative Server to perform the following operations:
1. Get the current date and time from the server
2. Evaluate the expression sent by the client
## [Lab 2](https://github.com/pranav610/Networks-Laboratory-CS39006/tree/main/2)
TCP and UDP Client-Server Programming:
1. UDP Client-Server to get the current date and time from the server.
2. Concurrent TCP Server to list the contents of a directory on the server with the following options:
    - Username and password authentication
    - List all files
    - Change directory
    - Open a directory
## [Lab 3](https://github.com/pranav610/Networks-Laboratory-CS39006/tree/main/3)
Load Balanced Server using TCP:
1. A client sends a request to the server to get the current date and time.
2. The server sends the request to one of the two servers (Server 1 and Server 2) according to the load on the servers and receives the response from the server.
3. The server sends the response back to the client.
## [Lab 4](https://github.com/pranav610/Networks-Laboratory-CS39006/tree/main/4)
HTTP Client-Server Programming:
1. HTTP Client to download a file from the server.
2. HTTP Server to serve the file requested by the client.
## [Lab 5](https://github.com/pranav610/Networks-Laboratory-CS39006/tree/main/5)
Message oriented communication using TCP:
1. Library to send and receive messages using TCP with single send and receive calls.
2. Custom socket library with following functions:
    - `my_socket()`
    - `my_bind()`
    - `my_listen()`
    - `my_accept()`
    - `my_connect()`
    - `my_send()`
    - `my_recv()`
    - `my_close()`
## [Lab 6](https://github.com/pranav610/Networks-Laboratory-CS39006/tree/main/6)
Raw Socket Programming:
1. Raw socket to find the route and estimate the latency and bandwidth of each link in the path.
    