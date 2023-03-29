#include "unix_wrappers.h"
/* utils */
char* itoa_s(int num){
    char* res = new char[20]();
    sprintf(res, "%d", num);
    return res;
}

/* show error msg relate to errno and exit*/
#define handle_error(msg) \
    do{                     \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)


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
int Open_clientfd(const char *hostname, int port){
    /* Obtain address(es) matching host/port */
    struct addrinfo hints;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    struct addrinfo* listp, *p;
    auto sport = itoa_s(port);

    Getaddrinfo(hostname, sport, &hints, &listp);
    
    int clientfd = -1;
    int rc = 0;
    /* try each address structures until we bind successfully. */
    for(p = listp; p != NULL; p = p->ai_next){
        if( (clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0 )
            continue; 
        
        if( (rc = connect(clientfd, p->ai_addr, p->ai_addrlen)) != -1){
            break;  /* Success */
        }   
 
        close(clientfd);
    }
    if(!p){
        perror("Error");
        exit(EXIT_FAILURE);
    }
    /* clean up */
    delete [] sport;
    freeaddrinfo(listp);

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
