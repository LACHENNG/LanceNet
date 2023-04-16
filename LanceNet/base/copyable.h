#ifndef BASE_COPYABLE
#define BASE_COPYABLE

namespace LanceNet
{

// tag class
// the derived class should have value type semantic;
class copyable{
protected:
    copyable() = default;
    ~copyable() = default;
};

} // namespace LanceNet;
#endif //!BASE_COPYABLE
