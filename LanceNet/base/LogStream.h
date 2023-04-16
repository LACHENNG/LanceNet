// Author: Lang Chen @ nwpu

#ifndef LancNet_BASE_LOGGING_LOG_STREAM_H
#define LancNet_BASE_LOGGING_LOG_STREAM_H

#include <string>
#include <LanceNet/base/noncopyable.h>
// Brief: A iostream-style memory buffer output stream, but no iostream usage which is too slow):
//  stdio is not type-safe, but fast
//  iostream is too slow , but type-safe
// The LogStream to combine benefits of these two

// MT-Attribute: Not thread-safe, the right way to use it is to construct a 
//     LogStream object for every log messsage, don`t warry about the performance as it is very cheap

// Algorithm
//     1. Interger to str -- using 'Mattew Wilson' Algorithm which is faster than stdio and iostream
//     2. float/double to str -- using 'Ryū: fast float-to-string conversion' Algorithm (currently the fastest float-to-string conversion algorithms by 2023) 

namespace LanceNet
{

class LogStream : noncopyable{
    using LogStreamBuffer = std::string;
    using self = LogStream;
public:
    // stream like operation for common bulit-in types
    self& operator<<(bool data);

    self& operator<<(short data);
    self& operator<<(unsigned short data);

    self& operator<<(int data);
    self& operator<<(unsigned int data);

    self& operator<<(long data);
    self& operator<<(unsigned long data);

    self& operator<<(long long data);
    self& operator<<(unsigned long long data);

    self& operator<<(float);
    self& operator<<(double);

    self& operator<<(const void* addr);
    self& operator<<(const char* pstr);
    self& operator<<(const std::string& str);

    // other utilities
    void append(const char* line);
    const LogStreamBuffer& buffer() const { return buf_; }
    void resetBuf();

private:
    LogStreamBuffer buf_;
};

// convenient for test purosed or integrated
// with cout
// Eg. cout << (LogStream() << data << " " << f) << endl;
template<typename ostream>
ostream& operator<<(ostream& os, LogStream& logs)
{
    os << logs.buffer();
    return os;
}

} // namespace LancNet

#endif // LancNet_BASE_LOGGING_LOG_STREAM_H
