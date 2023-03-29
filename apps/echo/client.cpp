#include "threadpool.h"
#include "unix_wrappers.h"
#include "msg.h"


int _main(int argc, char* argv[]);

int main(int argc, char* argv[]){
    int n_clients = 1; 
    if(argc == 4) n_clients = atoi(argv[3]);
    for(int i = 0; i < n_clients; i++){
        std::thread th(_main, argc, argv);
        th.detach();
    }
    getchar();
    return 0;
}
int _main(int argc, char* argv[])
{
    char hostname[12] = "localhost";

    int port = 46666;
    if(argc == 3)
        int port = atoi(argv[1]);
    if(argc == 4){
        strncpy(hostname, argv[2], 12);
    }
    printf("Connecting to %s : %d\n", hostname, port);
    
    int clientfd = Open_clientfd(hostname, port);
    printf("Connected to %s : %d\n", hostname, port);

    fd_set read_set;
    fd_set read_ready;

    char stdinBuf[4096]{"Hello message from client"};
    char msgBuf[4096];

    FD_ZERO(&read_set);
    FD_SET(0, &read_set);
    FD_SET(clientfd, &read_set);
    
    while(1){
        read_ready = read_set;
        // WARN: set timeval argument in select can lead to select return immediately which cause the outside loop busy!
        struct timeval tv{0,0};
        int nready = select(clientfd + 1, &read_ready, NULL, NULL, &tv);

        /* send LoginReq data*/
        
        int n = write(clientfd, stdinBuf, 4096);
        if(n == 0){
            printf("peer closed\n");
            break;
        }else if(n > 0){
            // printf("write %d bytes\n", n);
        }else{
            perror("write");
        }

        /* send data from stdin */
        if(nready > 0 && FD_ISSET(0, &read_ready)){
            int n = read(STDIN_FILENO, stdinBuf, 512);
            stdinBuf[n] = '\0';
            write(clientfd, stdinBuf, n);
        }

        /* read server message*/
        if(nready > 0 && FD_ISSET(clientfd, &read_ready)){
            int n = read(clientfd, msgBuf, 512);
            msgBuf[n] = 0;
            if(n > 0) printf("[Echo]: %s\n", msgBuf); 
            else if(n == 0){
                printf("server shutdown\n");
                return 0;
            }
            else{
                perror("read from server");
            }
        }
    }

    return 0;
}
