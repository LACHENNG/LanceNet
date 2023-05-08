#include <LanceNet/net/TimerId.h>
namespace LanceNet
{
namespace net
{



TimerId::TimerId()
  : timer_(nullptr),
    id_(-1)
{
}

TimerId::TimerId(Timer* timer, int64_t id)
 : timer_(timer),
    id_(id)
{
}

int64_t TimerId::id() const
{
    return id_;
}


} // namespace net
} // namespace LanceNet
