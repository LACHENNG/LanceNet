
#include <iostream>
#include "LanceNet/base/unix_wrappers.h"
using namespace std;
int main()
{
    int conn_fd = Open_clientfd("127.0.0.1", 2345);

    const char * msg = "hello, I am client";
    Write(conn_fd, msg, strlen(msg) + 1);

    Close(conn_fd);
    std::cout << "client exit" << endl;
    return 0;
}
