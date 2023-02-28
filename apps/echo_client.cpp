#include "threadpool.h"
#include "unix_wrappers.h"

int main(int argc, char* argv[])
{
    const char * hostname = "localhost";
    int port = atoi(argv[1]);

    int clientfd = Open_clientfd(hostname, port);
    printf("Connected to %s : %d\n", hostname, port);

    char Buf[1024];

    while(fgets(Buf, 1024, stdin) != NULL){
        printf("%s", Buf); 
        int rv = write(clientfd, Buf, strlen(Buf) + 1);
        if(rv < 0){
            perror("write");
            break;
        }
    }

    getchar();
    return 0;
}