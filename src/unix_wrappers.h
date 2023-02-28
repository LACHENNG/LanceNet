/*
 * Simple wrappers to common posix and unix system call with error checking
 */

#pragma once 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// bzero
#include <string.h>
#include <ctype.h>
// socket() listen()
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <errno.h>
#include <arpa/inet.h>
#include <memory>

/* defined by glic*/
// extern int errno;

typedef struct sockaddr SA;

/* show error msg relate to errno and exit*/
#define handle_error(msg) \
    do{                     \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)



/* utils */
char* itoa_s(int num);


/* sockets interface wrappers*/
int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const SA* addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);              
void Getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);

/* Wrappers for client/server helper functinos*/
int Open_clientfd(const char *hostname, int port);
int Open_listenfd(int port);

