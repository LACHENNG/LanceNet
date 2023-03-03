#include "threadpool.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unix_wrappers.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <assert.h>
using namespace std;

class Pool : public ThreadPool<int>{
public:
    Pool(size_t n_threads, size_t max_reqs, void* p_eventloop);
    virtual void run() override;

private:
    void* p_eventloop;

    void echo_from(int fd);
};


/*  the glibc implementation makes fd_set a fixed-size type, with FD_SETSIZE defined as 1024, and the FD_*() macros operating according to that limit.  
 *  To monitor  file  descriptors greater than 1023, use poll(2) instead
*/
class EchoSever{
public:

    EchoSever(int listen_port){

        listenfd = Open_listenfd(listen_port);
        printf("INFO: listening at port: %d\n", listen_port);

        FD_ZERO(&m_read);
        FD_ZERO(&m_ready);
        FD_SET(listenfd, &m_read);
        max_fd = listenfd;
    }

    void loop(){
        // m_thread_pool->lunch();

        while(1){
            m_ready = m_read;
            m_activefds.clear();
            /* watch fd [0 - max_fd + 1) to see when it has input*/
            nready = select(max_fd + 1, &m_ready, NULL, NULL, NULL); /* timeout set to NULL, block*/

            /* listenfd is readable, means new connection request*/
            struct sockaddr_storage peer_addr;
            socklen_t peerlen = sizeof(struct sockaddr_storage);
            if(FD_ISSET(listenfd, &m_ready)){
                int connfd = Accept4(listenfd, (SA *)&peer_addr, &peerlen, SOCK_NONBLOCK);

                /* client ip and port*/
                char ip[32], port[32];
                getnameinfo((SA*)&peer_addr, peerlen, ip, 32, port, 32, NI_NUMERICHOST | NI_NUMERICSERV);
                printf("New client %s:%s\n", ip, port);

                if(connfd >= FD_SETSIZE){
                    printf("Too many connections\n");
                    close(connfd);
                    continue;
                }
                FD_SET(connfd, &m_read);
                max_fd = max(max_fd, connfd);
                m_activefds.push_back(connfd);
            }

            /* response to the clients */
            char Buf[1024]; 
            for(int fd = 3; fd < max_fd && nready > 0; fd++, nready--){
                if(fd == listenfd || !FD_ISSET(fd, &m_ready)) continue; 

                int n = read(fd, Buf, 1024);
                Buf[n] = '\0';
                /* for nonblocking socket fd, n == 0 mean peer closed the connection*/
                if(n == 0){
                    FD_CLR(fd, &m_read);
                    close(fd);
                }
                else if(n == -1){
                    perror("read");
                }else if(n > 0){
                    fprintf(stdout, "from %d : %s\n", fd, Buf);
                    write(fd, Buf, n); 
                }       
            }
                
        }
    }

private:
    vector<int> m_activefds; 
    /* fd_set is a fixed size buffer.  Executing FD_CLR() or FD_SET() with a value of fd that is  negative or is equal to  or  larger  than FD_SETSIZE will result in undefined behavior*/
    fd_set m_read;
    fd_set m_ready;
    int max_fd;
    int listenfd;
    int nready;    
};



// Pool::Pool(size_t n_threads, size_t max_reqs, void* p_eventloop) : ThreadPool(n_threads, max_reqs){
//         this->p_eventloop = p_eventloop;
// }
    
// void Pool::run(){
//         while(1){
//             int connfd = m_sbuf->remove();
//             echo_from(connfd);
//         }
//     }


// void Pool::echo_from(int fd){
//     EchoSever* eventloop = (EchoSever*)p_eventloop;
//     // if(eventloop->m_fds[fd] == -1) return;

//     char Buf[1024]; 
//     int n = read(fd, Buf, 1024);
//     /* for nonblocking socket fd, n == 0 mean peer closed the connection*/
//     if(n == 0){
//         eventloop->m_fds[fd] = -1;
        
//         printf("read 0 bytes, peer closed the connection\n"); 
//     }
//     else if(n == -1){
//         perror("read");
//         assert(eventloop->m_fds[fd] == -1);  // unexpected / unhandle    
//     }else if(n > 0){
//         fprintf(stdout, "from %d : %s\n", fd, Buf);
//         // write(fd, Buf, n); /* issue: race condition  */ 
        
//     }      
// }




int main(){
    EchoSever server(46666);
    server.loop();

    return 0;
}