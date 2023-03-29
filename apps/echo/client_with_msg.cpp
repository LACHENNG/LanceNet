#include "threadpool.h"
#include "unix_wrappers.h"
#include "msg.h"
#include "pthread.h"

void* work(void* args);

int main(int argc, char* argv[]){
    int n_clients = 1; 
    if(argc == 4) n_clients = atoi(argv[3]);
    for(int i = 0; i < n_clients; i++){
        pthread_t tid;
        pthread_create(&tid, NULL, work, argv);
        pthread_detach(tid);
    }
    pthread_exit(0);
    
    // return 0;
}
void*  work(void* args)
{
    char hostname[12] = "localhost";

    int port = 46666;
    // if(argc == 3)
    //     port = atoi(argv[1]);
    // if(argc == 4){
    //     strncpy(hostname, argv[2], 12);
    // }

    int clientfd = Open_clientfd(hostname, port);
    printf("Connected to %s : %d\n", hostname, port);

    fd_set read_set;
    fd_set read_ready;

    FD_ZERO(&read_set);
    FD_SET(0, &read_set);
    FD_SET(clientfd, &read_set);
    
    while(1){
        read_ready = read_set;
      
        struct timeval tv{0,0};
        int nready = select(clientfd + 1, &read_ready, NULL, NULL, &tv);

        /* send LoginReq data*/
        MsgLoginReq login_msg;
        strncpy(login_msg.uName, "chenlang", 32);
        strncpy(login_msg.pwd, "abcdefg123", 32);

        int n = write(clientfd, (char*)&login_msg, sizeof(MsgLoginReq));
        if(n == 0){
            printf("peer closed\n");
            break;
        }else if(n > 0){
            // printf("write %d bytes\n", n);
        }else{
            perror("write");
        }
        
    }

    return 0;
}