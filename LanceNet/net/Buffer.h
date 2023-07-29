// Author: Lance @ nwpu

#ifndef LanceNet_NET_BUFFER_H
#define LanceNet_NET_BUFFER_H
#include "LanceNet/base/copyable.h"
#include <LanceNet/base/StringPiece.h>

#include <sys/types.h>
#include <vector>
#include <string>
#include <cassert>

namespace LanceNet
{

namespace net
{

constexpr const int kextra_buf_len = 65535;

// thread local buffer to optimize Buffer::readFd()
extern __thread char tl_extra_buf[kextra_buf_len];

// reference: Muduo Buffer
// init size kDefaultPrependSize + kDefaultInitSize
// implemented based on vector<char> which indicates that the capacity can automatically adjust to user`s needs

class Buffer : copyable
{
private:
    std::vector<char> buf_;
    size_t readindex_;
    size_t writeindex_;
    const int kinitSize_;
    const int kprependSize_;
    const int kdefaultInitSize_;
    constexpr static const char* const kCRLF = "\r\n" ;
    // optimize speeed : caching buf_.begin() 
    // has problem when involve std:move
    // void* cached_buf_begin_;

public:
    // default buffer size
    static const int kDefaultInitSize = 1024;
    // default pre-appendable size
    static const int kDefaultPrependSize = 8;

    Buffer(int initSize = kDefaultInitSize, int preappendSize = kDefaultPrependSize);

    // observal
    size_t readableBytes() const { return writeindex_ - readindex_; }
    size_t writeableBytes() const { return buf_.size() - writeindex_; }
    size_t prependableBytes() const { return readindex_; }
    // Note: not the size of contents, but the size of buffer
    size_t size() const { return buf_.size(); }
    size_t prependSize() const { return kprependSize_; }
    // get init size of the buffer
    size_t initSize() const { return kinitSize_; }

    // data still remains after peek
    const char* peek() const {
        assert(readindex_ < buf_.size() && readindex_ <= writeindex_);
        return beginRead();
    }
    // data still remains after peek
    int8_t peekInt8() const {
        assert(readableBytes() >= sizeof(int8_t));
        return *reinterpret_cast<const int8_t*>(peek());
    }
    // data still remains after peek
    int16_t peekInt16() const {
        assert(readableBytes() >= sizeof(int16_t));
        return *reinterpret_cast<const int16_t*>(peek());
    }
    // data still remains after peek
    int32_t peekInt32() const {
        assert(readableBytes() >= sizeof(int32_t));
        return *reinterpret_cast<const int32_t*>(peek());
    }
    // data still remains after peek
    int64_t peekInt64() const {
        assert(readableBytes() >= sizeof(int64_t));
        return *reinterpret_cast<const int64_t*>(peek());
    }

    // fetch `sz` bytes from the buffer
    // return is set to void to avoid misuse string str(retrieve(readableBytes()), readableBytes())
    // take out byte from buffer
    void retrieve(size_t size) { assert(size >= 0 && size <= readableBytes()); hasRead(size); }
    // take out byte from buffer
    void retrieveAll() { resetRWIndex(); }
    // take out byte from buffer
    void retrieveInt8() { retrieve(sizeof(int8_t)); }
    // take out byte from buffer
    void retrieveInt16() { retrieve(sizeof(int16_t)); }
    // take out byte from buffer
    void retrieveInt32() { retrieve(sizeof(int32_t)); }
    // take out byte from buffer
    void retrieveInt64() { retrieve(sizeof(int64_t)); }

    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);

    // read from network endian
    int8_t readInt8();
    // read from network endian
    int16_t readInt16();
    // read from network endian
    int32_t readInt32();
    // read from network endian
    int64_t readInt64();

    // read data from socket
    // optimized by "scatter input"
    // Deprecated
    ssize_t readFd(int fd);
    // read data from socket
    // optimized by "scatter input"
    // optimized version of readFd that use c array
    ssize_t readFdFast(int fd);

    void append(const char* data, size_t len);

    void append(const void* src, size_t len) { append(static_cast<const char*>(src), len); }
    void append(const StringPiece& str) { this->append(str.data(), str.size()); }
    void appendInt8(int8_t num) { append(&num, sizeof(int8_t)); }
    void appendInt16(int16_t num) { append(&num, sizeof(int16_t)); }
    void appendInt32(int32_t num) { append(&num, sizeof(int32_t)); }

    void preappend(const void*, size_t sz);

    void preappend(int64_t num) { preappend(&num, sizeof(int64_t)); }
    void preappend(int32_t num) { preappend(&num, sizeof(int32_t)); }
    void preappend(int16_t num) { preappend(&num, sizeof(int16_t)); }
    void preappend(int8_t num) { preappend(&num, sizeof(int8_t)); }

    // util to deal with text
    const char* findEOL() const; // \n
    const char* findEOL(const char* start) const;
    const char* findCRLF() const;      // \r\n
    const char* findCRLF(const char* start) const;
    void swap(Buffer& rhs);

    StringPiece toStringPiece() const { return StringPiece(beginRead(), readableBytes()); }

    void ensureWriteableSpace(size_t len);
    
    // adjust read watermark
    void hasRead(size_t sz) { 
        readindex_ += sz;
        if(readindex_ == writeindex_){ resetRWIndex(); }
    }
    // adjust write watermark
    void hasWritten(size_t sz) { writeindex_ += sz; }

private:
    // the begining of internal mem buffer
    char* begin() { return buf_.data(); }
    const char* begin() const { return buf_.data(); }
    char* beginRead() { return buf_.data() + readindex_; }
    const char* beginRead() const { return buf_.data() + readindex_; }
    char* beginWrite() { return buf_.data() + writeindex_; }
    const char* beginWrite() const { return buf_.data() + writeindex_; }
    void resetRWIndex() { readindex_ = writeindex_ = kprependSize_; }
    // unread watermark
    void unread(size_t sz) { readindex_ -= sz; assert(readindex_ >= 0); }

    void resize(size_t sz) { buf_.resize(sz); }

};

} // namespace net

} // namespace Lancenet

#endif // LanceNet_NET_BUFFER_H
