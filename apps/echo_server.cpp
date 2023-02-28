#include "threadpool.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unix_wrappers.h>

using namespace std;
std::mutex g_mutex;

class EchoServer : public ThreadPool<int>{
public:
    EchoServer(size_t pool_size, size_t max_reqs) : ThreadPool(pool_size, max_reqs){ }
    
    virtual void run() override{
        while(1){
            int connfd = m_sbuf->remove();
            echo_cnt(connfd);
        }
    }
private:
    void echo_cnt(int fd){
        char Buf[1024]; 
        /* WARN: the loop will be fucking busy as fd is set to nonblockin. Use IO multiplexing instead*/
        while (1)
        {
            int n = read(fd, Buf, 1024);
            /* for nonblocking socket fd, n == 0 mean peer closed the connection*/
            if(n == 0){
                cout << "Client closed the connection" << endl;
                break;
            }
            else if(n == -1){
                /* in that case, EAGAIN Indicates that there is no data in the receive buffer*/
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    continue;
                }

            }else if(n > 0){
                fprintf(stdout, "from %d : %s", fd, Buf);
                fflush(stdout);
            }
        }   
        close(fd);
    }
};

int main(int argc, char* argv[]){
    // int port = atoi(argv[1]);
    int port = 7777;
    if(argc == 2){
        port = atoi(argv[1]);
    }

    EchoServer EchoServer(8, 1000);
    EchoServer.lunch();

    int listenfd = Open_listenfd(port);
    Listen(listenfd, 1);
    printf("Listen at port: %d\n", port); 

    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

    /* Accept and Sever*/
    while(1){
        /* The addrlen argument is a value-result argument: the caller must initialize it to contain the size (in bytes) of the structure pointed to by addr;  on  return it will contain the actual size of the peer address. */
        int connfd = Accept4(listenfd, (SA*)&peer_addr, &peer_addr_len, SOCK_NONBLOCK); 
        cout << "Accepted new connection, fd: " << connfd << endl;
        EchoServer.add_task(connfd);
    }

    return 0;
}