
#ifndef BASE_NON_COPYABLE
#define BASE_NON_COPYABLE

namespace LanceNet
{

// tag class 
// the derived class should have object rather that value senmatic
class noncopyable{
protected:
    noncopyable() = default;
    ~noncopyable() = default;

public:
    /* copy ctor*/
    noncopyable(const noncopyable& ) = delete;
    /* copy assignment op*/
    noncopyable& operator=(const noncopyable& ) = delete;

};

} // namespace LanceNet
#endif //!BASE_NON_COPYABLE
