#ifndef BASE_UTILITY_H
#define BASE_UTILITY_H 

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

#endif //!BASE_UTILITY_H