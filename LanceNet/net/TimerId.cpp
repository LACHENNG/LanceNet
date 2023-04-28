#include <LanceNet/net/TimerId.h>
namespace LanceNet
{
namespace net
{

TimerId::TimerId()
  : timer_(nullptr),
    id_(0)
{
}

TimerId::TimerId(Timer* timer, int64_t id)
 : timer_(timer),
    id_(id)
{
}

} // namespace net
} // namespace LanceNet
