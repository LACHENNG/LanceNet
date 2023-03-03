#include "threadpool.h"
#include "unix_wrappers.h"

using namespace std; 
int main(int argc, char* argv[])
{
    
    const char * hostname = "localhost";
    int port = atoi(argv[1]);

    int clientfd = Open_clientfd(hostname, port);
    printf("Connected to %s : %d\n", hostname, port);

    char Buf[1024];
    while(fgets(Buf, 1024, stdin) != NULL){
        int rv = write(clientfd, Buf, strlen(Buf) + 1);
        if(rv < 0){
            perror("write");
            break;
        }else if(rv == 0){
            std::cout << "server closed" << endl;
            break;
        }
    }
    getchar();
    return 0;
}