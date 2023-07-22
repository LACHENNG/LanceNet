#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/base/Logging.h"
#include <asm-generic/socket.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/uio.h>
#include <netinet/tcp.h>

/* show error msg relate to errno and exit*/
#define handle_error(msg) \
    do{                     \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)

/* utils */
char* itoa_s(int num){
    char* res = new char[20]();
    sprintf(res, "%d", num);
    return res;
}


/* sockets interface wrappers*/
int Socket(int domain, int type, int protocol){
    int fd = socket(domain, type, protocol);
    if(fd == -1){
        handle_error("socket()");
    }
    return fd;
}

void Bind(int sockfd, const SA* addr, socklen_t addrlen){
    if(bind(sockfd, addr, addrlen)){
        handle_error("bind()");
    }
}

void Listen(int sockfd, int backlog){
    int rc = listen(sockfd, backlog);
    if(rc == -1){
        handle_error("listen()");
    }
}
int Accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags){
   int fd = accept4(sockfd, addr, addrlen, flags);
   if(fd == -1){
        perror("Accept4()");
        exit(EXIT_FAILURE);
   }
   return fd; 
}
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    int fd = accept(sockfd, addr, addrlen);
    if(fd == -1){
        perror("Accept4()");
        exit(EXIT_FAILURE);
    }
    return fd; 
}


/* Wrappers for client/server helper functinos*/
int Open_clientfd(const char *hostname, int port, bool nonblockfd, bool exitOnError){
    /* Obtain address(es) matching host/port */
    struct addrinfo hints;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* listp, *p;
    auto sport = itoa_s(port);

    Getaddrinfo(hostname, sport, &hints, &listp);

    int clientfd = -1;
    int rc = -1;
    /* try each address structures until we bind successfully. */
    for(p = listp; p != NULL; p = p->ai_next){
        if( (clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0 )
            continue;

        if( (rc = connect(clientfd, p->ai_addr, p->ai_addrlen)) != -1){
            break;  /* Success */
        }
        close(clientfd);
        clientfd = -1;
    }
    bool connectfailed = (p == nullptr);
    if(connectfailed){
        perror("Error");
       if(exitOnError) exit(EXIT_FAILURE);
    }
    /* clean up */
    delete [] sport;
    freeaddrinfo(listp);

    if(nonblockfd && !connectfailed)
    {
        int flags = fcntl (clientfd, F_GETFL, 0);
        if (flags == -1) {
            herror("fcntl");
        }
        flags |= (O_NONBLOCK | O_CLOEXEC);
        if (fcntl (clientfd, F_SETFL, flags) == -1) {
            herror("fcntl");
        }
    }
    return clientfd;
}

int Open_listenfd(int port){

    struct addrinfo hints;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* listp, *p;
    auto sport = itoa_s(port);

    Getaddrinfo(nullptr, sport, &hints, &listp);

    int sfd, optval = 1;
    int rc = 0;
    /* try each address structures until we bind successfully. */
    for(p = listp; p != NULL; p = p->ai_next){
        if((sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        /* Eliminates "Address already in use" error from bind" */
        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
        int optval = 1;
        setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int));

        if( (rc = bind(sfd, p->ai_addr, p->ai_addrlen)) == 0)
            break;             /* Success */

        close(sfd);
    }
    if(p == nullptr){
        perror("Error");
        exit(EXIT_FAILURE);
    }
    /* clean up */
    delete [] sport;
    freeaddrinfo(listp);

    /* Make it a listening socket willing to accept connection requests*/
    if(listen(sfd, 1024) < 0){
        perror("listen");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    return sfd;
}

void Getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res)
{
    int s = getaddrinfo(node, service, hints, res);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
}



void Shutdown(int __fd, int __how){
    int code = shutdown(__fd, __how);
    if(code < 0){
        handle_error("shutdown");
    }
}

int Epoll_create(int __size)
{
    int rc = epoll_create(__size);
    if(rc == -1){
        handle_error("epoll_create");
    }
    return rc;
}

int Epoll_create1(int __flags)
{
    int rc = epoll_create1(__flags);
    if(rc == -1){
        handle_error("epoll_create1");
    }
    return rc;
}

void Epoll_ctl(int __epfd, int __op, int __fd, epoll_event *event)
{
    int rc = ::epoll_ctl(__epfd, __op, __fd, event);
    if(rc == -1){
        handle_error("epoll_ctl");
    } 
}

int Epoll_wait(int __epfd, epoll_event *__events, int __maxevents, int __timeout)
{
    int nready = ::epoll_wait(__epfd, __events, __maxevents, __timeout);
    if(nready == -1){
        handle_error("epoll_wait");
    }
    return nready;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    int n = ::select(nfds, readfds, writefds, exceptfds, timeout);
    if(n == -1){
        handle_error("select");
    }
    return n;
}

struct sockaddr_in getPeerAddr(int sockfd) {
  struct sockaddr sa; 
  socklen_t addrlen = sizeof(struct sockaddr);

  if (getpeername(sockfd, &sa, &addrlen) < 0) {
    handle_error("getpeername failed");
  }
  return *((struct sockaddr_in *)(&sa));
}
int Timerfd_create(__clockid_t __clock_id, int __flags)
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);

    if(timerfd < 0){
        perror("timerfd_create()");
        exit(EXIT_FAILURE);
    }
    return timerfd;
}

void Timerfd_settime(int __ufd, int __flags, const struct itimerspec *newValue, struct itimerspec *oldValue)
{
    int code = ::timerfd_settime(__ufd, __flags, newValue, oldValue);
    if(code != 0)
    {
        handle_error("timerfd_settime");
    }
}

void Close(int fd)
{
    int code = ::close(fd);
    if(code != 0){
        handle_error("close()");
    }
}

//void Fcntl(){
//}
