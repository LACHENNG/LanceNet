/*
 * Description：Simple wrappers to common posix and unix system call with error checking
 * Author: Lang @ nwpu
 */

#pragma once
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <sys/uio.h>
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

#include <fcntl.h>

#include <sys/eventfd.h>
#include <sys/epoll.h>

#include <LanceNet/base/Logging.h>
/* defined by glic*/
// extern int errno;

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_IN;

/* show error msg relate to errno and exit*/
#define handle_error(msg)   \
    do{                     \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)



/* utils */
char* itoa_s(int num);

/* read write */
// try to inline to speed up
inline ssize_t Read(int fd, void *buf, size_t count){
    int nRead = read(fd, buf, count);

    if(nRead < 0 && nRead != EAGAIN){
        handle_error("read()");
    }
    return nRead;
}
inline ssize_t Write(int fd, const void *buf, size_t count){
    int nWrite = write(fd, buf, count);
    if(nWrite < 0 && (nWrite != EAGAIN && nWrite != EWOULDBLOCK))
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            return 0;
        }
        else handle_error("write()");
    }
    return nWrite;
}

inline ssize_t Readv(int __fd, const struct iovec *__iovec, int __count){
    auto n = ::readv(__fd, __iovec, __count);
    if(n == -1){
        LOG_WARN << "errno " << strerror(errno);
    }
    return n;
}

/* sockets interface wrappers*/
int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const SA* addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void Getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);
void Shutdown(int __fd, int __how);

/* epoll IO Multiplexing*/
int Epoll_create(int __size);
int Epoll_create1(int __flags);
void Epoll_ctl(int __epfd, int __op, int __fd, epoll_event *event);
int Epoll_wait(int __epfd, epoll_event *__events, int __maxevents, int __timeout);

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *__restrict __timeout);
/* sockops */
struct sockaddr_in getPeerAddr(int sockfd);

/* Wrappers for client/server helper functinos*/
// return a nonblock fd
int Open_clientfd(const char *hostname, int port, bool nonblockfd= true, bool exitOnError = true);

int Open_listenfd(int port);


int Timerfd_create(__clockid_t __clock_id, int __flags);
void Timerfd_settime(int __ufd, int __flags, const struct itimerspec *newValue, struct itimerspec *oldValue);

void Close(int fd);

