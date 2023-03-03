#include "threadpool.h"
#include "unix_wrappers.h"

int main(int argc, char* argv[])
{
    const char * hostname = "localhost";

    int port = 46666;
    if(argc == 2)
        int port = atoi(argv[1]);

    int clientfd = Open_clientfd(hostname, port);
    printf("Connected to %s : %d\n", hostname, port);

    fd_set ready;

    while(1){
        FD_ZERO(&ready);
        FD_SET(0, &ready);
        FD_SET(clientfd, &ready);

        
        // WARN: set timeval argument in select can lead to select return immediately which cause the outside loop busy!
        // struct timeval tv{0,0};
        int nready = select(clientfd + 1, &ready, NULL, NULL, NULL);

        char stdinBuf[512];
        char msgBuf[512];
        /* stdin */
        if(nready > 0 && FD_ISSET(0, &ready)){
            int n = read(STDIN_FILENO, stdinBuf, 512);
            stdinBuf[n] = '\0';
            write(clientfd, stdinBuf, n);
        }
        /* server message*/
        if(nready > 0 && FD_ISSET(clientfd, &ready)){
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

    char Buf[1024];

    while(fgets(Buf, 1024, stdin) != NULL){
        // printf("%s", Buf); 
        int rv = write(clientfd, Buf, strlen(Buf) + 1);
        if(rv < 0){
            perror("write");
            break;
        }
        
        char Buf2[1024];
        int n = read(clientfd, Buf2, 1024);
        Buf2[n] = 0;
        printf("echo: %s\n", Buf2);
    }

    getchar();
    return 0;
}