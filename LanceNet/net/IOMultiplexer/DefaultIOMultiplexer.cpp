#include "LanceNet/net/EventLoop.h"
#include <LanceNet/net/IOMultiplexer.h>
#include <LanceNet/net/IOMultiplexer/Epoller.h>
#include <LanceNet/net/IOMultiplexer/Poller.h>
#include <LanceNet/net/IOMultiplexer/Selecter.h>
#include <cstdlib>


namespace LanceNet
{
namespace net
{

IOMultiplexer* IOMultiplexer::getDefaultIOMultiplexer(EventLoop* loop)
{
    if(::getenv("LanceNet_USE_POLL")){
        return new Poller(loop);
    }else if(::getenv("LanceNet_USE_SELECT")){
        return new Selecter(loop); 
    }
    return new Epoller(loop);
}

}
}

