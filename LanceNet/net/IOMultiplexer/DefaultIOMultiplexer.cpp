#include "LanceNet/net/EventLoop.h"
#include <LanceNet/net/IOMultiplexer.h>
#include <LanceNet/net/IOMultiplexer/Epoller.h>
#include <LanceNet/net/IOMultiplexer/Poller.h>
#include <LanceNet/net/IOMultiplexer/Selecter.h>
#include <LanceNet/net/IOMultiplexer/PollerX.h>
#include <LanceNet/base/Logging.h>
#include <cstdlib>


namespace LanceNet
{
namespace net
{
class StartUpInfo{
public:
    StartUpInfo(){
        LOG_INFOC << "IOMultiplexer - Set environment variable " 
                 << "LanceNet_USE_EPOLL, LanceNet_USE_SELECT, LanceNet_USE_POLLX, LanceNet_USE_POLL (default) to use different IOMultiplexer";
    }
};
static StartUpInfo info;

IOMultiplexer* IOMultiplexer::getDefaultIOMultiplexer(EventLoop* loop)
{
    if(::getenv("LanceNet_USE_EPOLL")){
        return new Epoller(loop);
    }else if(::getenv("LanceNet_USE_SELECT")){
        return new Selecter(loop);
    }else if(::getenv("LanceNet_USE_POLLX")){
        return new PollerX(loop);
    }
    return new Poller(loop);
}

}
}

