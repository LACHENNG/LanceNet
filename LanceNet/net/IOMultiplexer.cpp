#include <LanceNet/net/IOMultiplexer.h>
#include <LanceNet/net/EventLoop.h>

namespace LanceNet
{
namespace net
{

IOMultiplexer::IOMultiplexer(EventLoop* loop)
  :  owner_loop_(loop)
{
}

IOMultiplexer::~IOMultiplexer()
{};

void IOMultiplexer::assertInEventLoopThread()
{
    owner_loop_->assertInEventLoopThread();
}

} // namespace net
} // namespace LanceNet


