
// Chat client that receive msg from others and read console input and send to others
//
#include "LanceNet/base/CountDownLatch.h"
#include "LanceNet/base/Mutex.h"
#include "LanceNet/base/Thread.h"
#include "LanceNet/net/EventLoop.h"
#include "chat_client.h"
#include <string>
#include <iostream>
using namespace std;

CountDownLatch latch(1);
ChatClient* chatClient = nullptr;

void input()
{
    latch.wait();
    string msg;
    // FIXME : set connection to nullptr at disconnection
    while(cin >> msg && chatClient) {
        cout << "get input msg .size() = " << msg.size();
        chatClient->send(msg);
    }
}

void OnNewConnection(const TcpConnectionPtr& tcpConnPtr, int conn_fd, const SA_IN* peer_addr)
{
    latch.countdown();
}


int main()
{
    base::Thread inputThread(input, "read  console input thread");
    inputThread.start();

    EventLoop loop;
    chatClient = new ChatClient(&loop, "localhost", 2345);
    chatClient->setOnConnectedCb(OnNewConnection);
    chatClient->start();

    loop.StartLoop();
}
