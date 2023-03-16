/* 百万并发的一个多线程的基于生产者消费者的select 服务端架构， 对于新客户加入事件，主线程监听连接请求并将其平分到多个消费者消息队列，并由消费者线程进行处理
 * 对于客户退出事件，同样使用一个消息队列，对主线程进行通知，子线程产生退出事件消息给主线程，在linux下最大支持1024个客户连接
*/
#include "echoserver.h"
#include "workers.h"
using namespace std;


/*  the glibc implementation makes fd_set a fixed-size type, with FD_SETSIZE defined as 1024, and the FD_*() macros operating according to that limit.  
 *  To monitor  file  descriptors greater than 1023, use poll(2) instead
*/


int main(){
    unique_ptr<EchoServer> upEchoServer = make_unique<EchoServer>(46666);
    upEchoServer->loop();
    
    cout << "main thread exit\n"; 
    return 0;
}