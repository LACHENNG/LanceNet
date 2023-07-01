#include "LanceNet/base/Time.h"
#include "LanceNet/base/unix_wrappers.h"
#include <LanceNet/net/Connector.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>

#include <assert.h>
namespace LanceNet
{
namespace net
{

Connector::Connector(EventLoop* loop, int port, std::string hostname)
  : port_(port),
    hostname_(std::move(hostname)),
    next_retryInterval_(0.5),
    started_(false),
    stop_(false),
    loop_(loop)
{
}

Connector::~Connector()
{
}

void Connector::setConnectionCallback(ConnectionCallback cb)
{
    cb_ = cb;
}

void Connector::start()
{
    if(stop_){
        return ;
    }
    assert(!started_);
    int connfd = Open_clientfd(hostname_.data(), port_, true, false);
    if(connfd < 0){
        TimeStamp nextTry = TimeStamp::now() + next_retryInterval_;
        LOG_INFOC << "Connect to hostname : " << hostname_ << "failed, retry at " << nextTry.toFmtString();
        loop_->runAt(nextTry,
            std::bind(&Connector::restart, this));
    }else{
        assert(cb_);
        cb_(connfd);
    }
}

void Connector::stop()
{
    stop_ = true;
}

void Connector::restart()
{
    next_retryInterval_ *=  2;
    // limits max retry interval to 30s
    if(next_retryInterval_ > 30){
        next_retryInterval_ = 30;
    }
    start();
}

} // namespace net
} // namespace LanceNet
