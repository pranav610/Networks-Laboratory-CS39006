# MESSAGE ORIENTED TCP

## Data Structures Used

```c
struct MyTable{
    int *sizes; // integer array to store the size of the strings stored in the table
    char **table; // array of strings to store messages to send (given  by  the  my_send  call)  and  messages  received  (to  be  read  by  the  my_recv call
    int in; // the pointer to the table for storing the entry point to the table
    int out; // the pointer to the table for removing the messages from the table
    int connected; // boolean variable to check if the connection is established or not, used to avoid recv call before the connection is established        
}
```
    
## Function Declaration Used:

### `int my_socket(int, int, int);`

- **Parameters:**
    - `domain`: The domain of the socket.
    - `type`: The type of the socket.
    - `protocol`: The protocol for the socket. (Currently, only `SOCK_MyTCP` is supported.)

- **Returns:**
    - `int`: The socket file descriptor for Message Oriented TCP.

- **Description:**
  This function is a wrapper around the standard TCP socket creation function. The function creates the socket and initializes the `Send_Message` and `Received_Message` tables, as well as the mutexes and conditional variables used in the threads. After the initialization, it creates and starts the execution of the `S` and `R` threads.


### `int my_bind(int, const struct sockaddr *, socklen_t);`
- **Parameters:**
    - `sockfd`: The socket file descriptor.
    - `addr`: The pointer to the `sockaddr` structure.
    - `addrlen`: The size of the `sockaddr` structure.
- **Returns:**
    - `int`: 0 on success, -1 on failure.
- **Description:**
    This function is a wrapper around the standard TCP bind function.

### `int my_listen(int, int);`
- **Parameters:**
    - `sockfd`: The socket file descriptor.
    - `backlog`: The maximum length to which the queue of pending connections for sockfd may grow.
- **Returns:**
    - `int`: 0 on success, -1 on failure.
- **Description:**
    This function is a wrapper around the standard TCP listen function.

### `int my_accept(int, struct sockaddr *, socklen_t *);`
- **Parameters:**
    - `sockfd`: The socket file descriptor.
    - `addr`: The pointer to the `sockaddr` structure.
    - `addrlen`: The size of the `sockaddr` structure.
- **Returns:**
    - `int`: The socket file descriptor for the accepted connection.
- **Description:**
    This function is a wrapper around the standard TCP accept function. If the accept call is successful it assignes the boolean variable connected to 1 by locking the mutex_r.

### `int my_connect(int, const struct sockaddr *, socklen_t);`
- **Parameters:**
    - `sockfd`: The socket file descriptor.
    - `addr`: The pointer to the `sockaddr` structure.
    - `addrlen`: The size of the `sockaddr` structure.
- **Returns:**
    - `int`: 0 on success, -1 on failure.
- **Description:**
    This function is a wrapper around the standard TCP connect function. If the connect call is successful it assignes the boolean variable connected to 1 by locking the mutex_r.

### `int my_send(int, const void *, size_t, int);`
- **Parameters:**
    - `sockfd`: The socket file descriptor.
    - `buf`: The pointer to the data to be sent.
    - `len`: The size of the data to be sent.
    - `flags`: The flags to be used in the send call.
- **Returns:**
    - `int`: The number of bytes sent on success, -1 on failure.
- **Description:**
    The function begins it s execution by locking mutex_s then it blocks until the Send_Message table does not have a free entry. If a free entry is found in the Send_Message table it puts the data in the table and returns immediately.

### `int my_recv(int, void *, size_t, int);`
- **Parameters:**
    - `sockfd`: The socket file descriptor.
    - `buf`: The pointer to the data to be received.
    - `len`: The size of the data to be received.
    - `flags`: The flags to be used in the recv call.
- **Returns:**
    - `int`: The number of bytes received on success, -1 on failure.
- **Description:**
    The function begins it s execution by locking mutex_r then it blocks until the Received_Message table has some data to be read. If the Received_Message table has some data to be read it reads the data from the table and returns immediately. In case of of connection close it returns -1.

### `int my_close(int);`
- **Parameters:**
    - `sockfd`: The socket file descriptor.
- **Returns:**
    - `int`: 0 on success, -1 on failure.
- **Description:**
    This function is a wrapper around the standard TCP close function. It first starts it execution by sleep call for 5 secs to avoid race conditions. Then it sends the cancellation request to the S and R threads and then it closes the socket file descriptor. The function also frees the memory allocated to the Send_Message and Received_Message tables.

## Thread Declaration Used:
### `void *S(void *);`
- **Parameters:**
    - `arg`: An optional argument.
- **Returns:**
    - `void *`: A void pointer.
- **Description:**
    The thread S sleeps periodically for a time T and periodically checks if there is any message to be sent in the Send_Message table. If there is a message to be sent it takes out the message(s) from the table and sends it using the standard TCP send call.

### `void *R(void *);`
- **Parameters:**   
    - `arg`: An optional argument.
- **Returns:**
    - `void *`: A void pointer.
- **Description:**
    The thread R periodically checks if there is any message to be received in the Received_Message table. If there is a message to be received it takes out the message(s) from the table and returns it using the standard TCP recv call. If the recv call returns 0 or -1 it will inform the my_recv function that the connection is closed by setting the boolean variable connected to 0 by locking the mutex_r.

## Addional variables defined:
### `struct Mytable Send_Message, Received_Message;`
- **Description:**
    The struct variable to store the messages to be sent and the messages that are received.
### `pthread_t tid_s, tid_r;`
- **Description:**
    The variable to store the thread ids of the S and the R threads respectively.
### `int __sockfd;`
- **Description:**
    This variable is used to store the socket file descriptor returned by the socket call. This socket descriptor gets updated on the my_accept call to the new socket descriptor returned by the standard TCP accept call.
### `int __oldsockfd;`
- **Description:**
    This variable also stores the socket file descriptor returned by the socket call, but when my_accept is called it still remains equal to the previous socket descriptor file. This is for the proper functioning of the my_close call, so that on closing new socket the tables do not get deleted.
### `pthread_mutex_s, mutex_r;`
- **Description:**
    The mutex variables to gain the mutual exclusive access of the Send_Message and the Received_Message tables.
### `pthread_cond_t cond_s, cond_r;`
- **Description:**
    The conditional variables to block the S and R threads and the my_send and my_recv functions when the Send_Message and Received_Message tables are full or empty. The cond_s is for the conditional variable between the my_send function and S thread, whereas the cond_r is for the conditional variable between the my_recv function and R thread.