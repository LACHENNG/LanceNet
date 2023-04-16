// Author: Lance @ nwpu

#ifndef LanceNet_NET_BUFFER_H
#define LanceNet_NET_BUFFER_H
#include "LanceNet/base/copyable.h"
#include <LanceNet/base/StringPiece.h>

#include <vector>
#include <string>

namespace LanceNet
{

namespace net
{
// reference: Muduo Buffer
// init size kDefaultPrependSize + kDefaultInitSize
// implemented based on vector<char> which indicates that the capacity can automatically adjust to user`s needs

class Buffer : copyable
{
private:
    std::vector<char> buf_;
    size_t readindex_;
    size_t writeindex_;

    constexpr static const char* const kCRLF = "\r\n" ;
public:
    // default buffer size
    static const int kDefaultInitSize = 1024;
    // default pre-appendable size
    static const int kDefaultPrependSize = 8;

    Buffer(int initSize = kDefaultInitSize, int preappendSize = kDefaultPrependSize);

    // observal
    size_t readableBytes() const;
    size_t writeableBytes() const;
    size_t prependableBytes() const;

    // peek data(does not affect internal state)
    const char* peek() const;
    int8_t peekInt8() const;
    int16_t peekInt16() const;
    int32_t peekInt32() const;
    int64_t peekInt64() const;


    // fetch `sz` bytes from the buffer
    // return is set to void to avoid misuse string str(retrieve(readableBytes()), readableBytes())
    void retrieve(size_t sz);
    void retrieveAll();
    void retrieveInt8();
    void retrieveInt16();
    void retrieveInt32();
    void retrieveInt64();
    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);

    // read from network endian
    int8_t readInt8();
    int16_t readInt16();
    int32_t readInt32();
    int64_t readInt64();

    void append(const void* data, size_t len);
    void append(const char* data, size_t len);
    void append(const StringPiece& str);
    void appendInt8(int8_t);
    void appendInt16(int16_t);
    void appendInt32(int32_t);

    void preappend(const void*, size_t sz);
    void preappend(int64_t);
    void preappend(int32_t);
    void preappend(int16_t);
    void preappend(int8_t);

    // util to deal with text
    const char* findEOL() const; // \n
    const char* findEOL(const char* start) const;
    const char* findCRLF() const;      // \r\n
    const char* findCRLF(const char* start) const;
    void swap(Buffer& rhs);

    StringPiece toStringPiece() const;

private:
    // int readFd(int);
    char* begin();
    const char* begin() const;
    char* beginRead();
    const char* beginRead() const;
    char* beginWrite();
    const char* beginWrite() const;
    // used to modify
    void hasRead(size_t);
    void hasWritten(size_t);
    void unread(size_t);

    void ensureWriteableSpace(size_t len);
};

} // namespace net

} // namespace Lancenet

#endif // LanceNet_NET_BUFFER_H
