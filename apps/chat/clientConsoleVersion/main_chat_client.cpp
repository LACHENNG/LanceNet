
// Chat client that receive msg from others and read console input and send to others
//
#include "LanceNet/base/CountDownLatch.h"
#include "LanceNet/base/Mutex.h"
#include "LanceNet/base/Thread.h"
#include "LanceNet/net/EventLoop.h"
#include "chat_client.h"
#include <string>
#include <iostream>
#include "../codec/msg.pb.h"

using namespace std;

CountDownLatch latch(1);
ChatClient* chatClient = nullptr;

// thread func to read inputs from console 
void input()
{
    latch.wait();
    DialogMessage msg;
    cout << DialogMessage::GetDescriptor()->name();  
    // FIXME : set connection to nullptr at disconnection
    string username = "test";
    int type = 1;
    string content;
    while(getline(cin, content) && chatClient) {
        msg.set_username(username);
        msg.set_type(type);
        msg.set_content(content);
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
