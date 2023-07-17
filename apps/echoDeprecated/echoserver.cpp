#include "echoserver.h"
#include "workers.h"
#include "clientsession.h"
#include "mutex"
#include "algorithm"
bool cmp(ClientSession* a, ClientSession* b){
    return a->getFd() < b->getFd();
}

EchoServer::EchoServer(int listen_port) 
        :m_bRuning(true), m_UIThread(std::mem_fn(&EchoServer::UI), this),
         m_pworkers(new Workers<ClientSession*>(8, 1024, this))
{
    /* console UI*/
    m_UIThread.detach();
    /* listen fd */
    listenfd = Open_listenfd(listen_port);
    printf("INFO: listening at port: %d\n", listen_port);

    /* eventfd notifying exitsten't from worker to Server*/
    m_eventfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);

    FD_ZERO(&m_read);
    FD_ZERO(&m_ready);
    FD_SET(listenfd, &m_read);
    FD_SET(m_eventfd, &m_read);
    
    // max_fd = max(listenfd, m_eventfd);
    auto listenSess = new ClientSession(listenfd, this);
    auto m_eventSess = new ClientSession(m_eventfd, this);
    m_maxheap.push(listenSess);
    m_maxheap.push(m_eventSess);

    m_active_clients.insert(listenSess);
    m_active_clients.insert(m_eventSess);
}

EchoServer::~EchoServer(){

}


void EchoServer::loop(){
    start();

    m_timer.update(); 
    int nready{0}; 
    while(m_bRuning){
        
        m_ready = m_read;
        
        /* watch fd [0 - max_fd + 1) to see when it has input*/
        struct timeval tv{1, 0};
        nready = select(m_maxheap.top()->getFd() + 1, &m_ready, NULL, NULL, &tv); /* timeout set to NULL, block*/

        /* listenfd is readable, new connection request*/
        struct sockaddr_storage peer_addr;
        socklen_t peerlen = sizeof(struct sockaddr_storage);
        if(FD_ISSET(listenfd, &m_ready)){
            if(m_active_clients.size() >= FD_SETSIZE){
                printf("Too many  connections\n");
                exit(0);
                continue;
            }

            int connfd = Accept4(listenfd, (SA *)&peer_addr, &peerlen, SOCK_NONBLOCK);

            /* client ip and port*/
            char ip[32], port[32];
            getnameinfo((SA*)&peer_addr, peerlen, ip, 32, port, 32, NI_NUMERICHOST | NI_NUMERICSERV);
            printf("New client from %s:%s\n", ip, port);

            FD_SET(connfd, &m_read);
            auto newSession = new ClientSession(connfd, this);
            m_maxheap.push(newSession);
            m_active_clients.insert(newSession);
            ++total_conn;  
        }

        /* add task to the workers */
        // for(int fd = 3; fd <= max_fd && nready > 0; fd++){
        /* pick the readable fd in all active(connected) */
        for(auto session : m_active_clients){
            if(nready-- <= 0) break;
            int clientfd = session->getFd();
            if(clientfd != listenfd  && clientfd != m_eventfd 
                && FD_ISSET(clientfd, &m_ready)){
                /* exit sockets should`nt be added again */
                m_pworkers->addTask(session);
                // nready--;
            }         
        } 

        /* client exits*/
        if(FD_ISSET(m_eventfd, &m_ready)){
            // TODO: double check to save resouces
            decltype(m_exit_msgs) exitClients;
            if(m_exit_msgs.size() > 0){
                // {
                std::lock_guard<std::mutex> lock(m_mutex_existed_fds);
                exitClients = std::move(m_exit_msgs);
                m_exit_msgs.clear();
                // }
            }

            /* clear event */
            int event_val;
            eventfd_t eval;
            eventfd_read(m_eventfd, &eval);
    
            /* clear fd_set and active fds*/
            for(auto client : exitClients){
                if(m_active_clients.find(client) != m_active_clients.end()){
                    FD_CLR(client->getFd(), &m_read);
                    m_active_clients.erase(client);
                    total_closed++;
                    int rv = close(client->getFd());
                    if(rv == -1){
                        perror("close");
                        exit(0);
                    }
                    while(!m_maxheap.empty() && m_active_clients.find(m_maxheap.top()) == m_active_clients.end()){
                        m_maxheap.pop();
                    }
                }
            }
        }

        /* show statistics*/
        auto t1 = m_timer.getElapsedSecond();
        if(t1 > 1.0){
            // printf("Total connectd : [%d] | ToTal Closed: [%d]\n", total_conn.load(), total_closed.load());
            printf("Total connectd : [%d] \n", total_conn.load());
            printf("Active clients<%lu>, recvRate<%d> bytes/sec\n",m_active_clients.size(), static_cast<int>(m_totalbytes / t1));
            // for_each(m_active_clients.begin(), m_active_clients.end(), [](ClientSession* client){
            //     printf("%d ", client->getFd());
            // });
            printf("maxFd：%d\n size: %lu ",m_maxheap.top()->getFd(), m_maxheap.size());
            m_totalbytes = 0;
            m_timer.update();
        }
    }
}

/* start and stop server */
void EchoServer::start(){
    m_bRuning = true;
}
void EchoServer::stop(){
    m_bRuning = false; 
}

/* invoked by workers */
void EchoServer::OnClientExit(ClientSession* client){   
    cout << "client exit\n"; 
    // cout << " fd exit: " << fd << " thread sleep: " <<std::this_thread::get_id() << endl;
    {
        std::lock_guard<std::mutex> lk(m_mutex_existed_fds);
        m_exit_msgs.insert(client);
    }
    /* make the event fd readable */
    eventfd_t val{1};
    eventfd_write(m_eventfd, val);
}
void EchoServer::addRecvBytes(size_t add_cnt){
    m_totalbytes += add_cnt;
}
/* console user interface */
void EchoServer::UI(){
    std::string cmd;
    while(getline(cin, cmd)){
        if(cmd == "exit"){
            stop();
            return ;
        }else{
            std::cout << "Unrecognized cmd" << std::endl;
        }
        cout << "ui active\n";
    }
}



