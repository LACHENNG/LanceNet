#include <LanceNet/net/Buffer.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/base/unix_wrappers.h>
#include <bits/stdint-intn.h>
#include <cstring>
#include <memory.h>
#include <assert.h>
#include <algorithm>
namespace LanceNet
{
namespace net
{

// default buffer size
const int Buffer::kDefaultInitSize;
// default pre-appendable size
const int Buffer::kDefaultPrependSize;

__thread char tl_extra_buf[kextra_buf_len];

Buffer::Buffer(int initSz, int prependSz)
  : buf_(initSz + prependSz),
    readindex_(prependSz),
    writeindex_(prependSz),
    kinitSize_(initSz),
    kprependSize_(prependSz),
    kdefaultInitSize_(initSz)
{
}

/////////////////////////////////////////////////////////
/// retrieve
/////////////////////////////////////////////////////////

std::string Buffer::retrieveAllAsString()
{
    std::string rv = std::string(buf_.begin() + readindex_, buf_.begin() + writeindex_);
    retrieve(readableBytes());
    return rv;
}

std::string Buffer::retrieveAsString(size_t len)
{
    assert(len <= readableBytes());
    std::string rv(beginRead(), beginRead() + len);
    retrieve(len);
    // CppCoreGuidelines ES.56
    // return std::move(rv); // bad
    return rv;
}

/////////////////////////////////////////////////////////
/// read
/////////////////////////////////////////////////////////

int8_t Buffer::readInt8()
{
    assert(readableBytes() >= sizeof(int8_t));
    int8_t* rv = reinterpret_cast<int8_t*>(beginRead());
    hasRead(sizeof(int8_t));
    return *rv;
}

int16_t Buffer::readInt16()
{
    assert(readableBytes() >= sizeof(int16_t));
    int16_t* rv = reinterpret_cast<int16_t*>(beginRead());
    hasRead(sizeof(int16_t));
    return *rv;

}

int32_t Buffer::readInt32()
{
    assert(readableBytes() >= sizeof(int32_t));
    int32_t* rv = reinterpret_cast<int32_t*>(beginRead());
    hasRead(sizeof(int32_t));
    return *rv;

}

int64_t Buffer::readInt64()
{
    assert(readableBytes() >= sizeof(int64_t));
    int64_t* rv = reinterpret_cast<int64_t*>(beginRead());
    hasRead(sizeof(int64_t));
    return *rv;
}

// read data from fd to internal buffer
// Deprecated
ssize_t Buffer::readFd(int fd)
{
    std::vector<struct iovec> iovs(2, {nullptr, 0});
    struct iovec iov1, iov2;

    iov1.iov_base = beginWrite();
    iov1.iov_len = writeableBytes();
    iov2.iov_base = tl_extra_buf;
    iov2.iov_len = kextra_buf_len;

    iovs[0] = iov1;
    iovs[1] = iov2;

    auto nBytesRead = Readv(fd, iovs.data(), iovs.size());
    // if no data read into extra buffer
    if(nBytesRead <= static_cast<ssize_t>(iov1.iov_len))
    {
        hasWritten(nBytesRead);
    }
    else // has data read into extra buffer
    {
        hasWritten(iov1.iov_len);
        int extra_len = nBytesRead - iov1.iov_len;
        assert(extra_len > 0 && extra_len <= static_cast<ssize_t>(iov2.iov_len));
        append(iov2.iov_base, extra_len);
    }
    return nBytesRead;
}

ssize_t Buffer::readFdFast(int fd)
{
    struct iovec iovs[2];
    // static std::vector<struct iovec> iovs(2, {nullptr, 0});
    iovs[0].iov_base = beginWrite();
    iovs[0].iov_len = writeableBytes();
    iovs[1].iov_base = tl_extra_buf;
    iovs[1].iov_len = kextra_buf_len;

    auto nBytesRead = Readv(fd, iovs, sizeof(iovs)/sizeof(*iovs));

    // if no data read into extra buffer
    if(nBytesRead <= static_cast<ssize_t>(iovs[0].iov_len))
    {
        hasWritten(nBytesRead);
    }
    else // has data read into extra buffer
    {
        hasWritten(iovs[0].iov_len);
        int extra_len = nBytesRead - iovs[0].iov_len;
        assert(extra_len > 0 && extra_len <= static_cast<ssize_t>(iovs[1].iov_len));
        append(iovs[1].iov_base, extra_len);
    }
    return nBytesRead;

}
////////////////////////////////////////////////////////
// append
////////////////////////////////////////////////////////

void Buffer::append(const char* src, size_t len)
{
    ensureWriteableSpace(len);
    std::copy(src, src + len, beginWrite());
    hasWritten(len);
}

void Buffer::ensureWriteableSpace(size_t len)
{
    if(writeableBytes() >= len){
        return ;
    }

    // move data to the front, make space inside buffer
    if(writeableBytes() + prependableBytes() >= len + kprependSize_){
        size_t readable = readableBytes();
        memmove(begin() + kprependSize_, beginRead(), readableBytes());
        readindex_ = kprependSize_;
        writeindex_ = readindex_ + readable;

    }
    else {
        buf_.resize( writeindex_ + len);
    }
}


// void Buffer::append(const void* src, size_t len)
// {
//     size_t writeable = writeableBytes();
//     size_t prependable = prependableBytes();
//     // if a internal buffer movement can meet the required space, move and avoid direct append
//     if(writeable < len &&
//        writeable + prependable >= len + kprependSize_)
//     {
//         assert(readindex_ <= writeindex_ && writeindex_ <= buf_.size());
//         // step 1: move the buffer to the begining of buf_
//         memmove(&buf_[kDefaultInitSize], beginRead(), readableBytes());
//         // step 2: update watermark
//         int offset = readindex_ - kprependSize_;
//         readindex_ -= offset;
//         writeindex_ -= offset;
//         // step 3: append data
//         assert(writeindex_ + len <= buf_.size());
//     }
//     else
//     {
//         // expand buf to get enough memory
//         size_t newSize = writeindex_ + len;
//         if(newSize > buf_.size()){
//             buf_.resize(newSize);
//         }
//     }
//     // now append to the end
//     memcpy(&buf_[writeindex_], src, static_cast<size_t>(len));
//         writeindex_ += len;
// }

void Buffer::preappend(const void * src, size_t len)
{
    assert(len <= prependableBytes());
    assert(beginRead() - len >= begin());
    memcpy(beginRead() - len, src, len);
    unread(len);
}

//////////////////////////////////////////////////
/// util to deal with text buffer
/////////////////////////////////////////////////j

const char* Buffer::findEOL(const char* start) const
{
    // Wrong
    // const char* pos = strchr(peek(), '\n');
    //
    assert(start >= peek());
    assert(start < beginWrite());
    const char* begin = peek();
    const char* end = begin + readableBytes();
    if(start >= end)
    {
        return NULL;
    }
    auto eol = std::find(begin, end, '\n');
    if(eol == end){
        eol = NULL;
    }
    // Wrong
    // const void* pos = memchr(start, '\n', readableBytes());
    return static_cast<const char*>(eol);
}

const char* Buffer::findEOL() const
{
    return findEOL(peek());
}

const char* Buffer::findCRLF() const
{
    return findCRLF(peek());
}

const char* Buffer::findCRLF(const char* start) const
{
    const char* crlf = static_cast<const char*>(memmem(start, readableBytes(), kCRLF, strlen(kCRLF)));
    return crlf;
}

void Buffer::swap(Buffer& that)
{
    buf_.swap(that.buf_);
    std::swap(readindex_, that.readindex_);
    std::swap(writeindex_, that.writeindex_);
}

} // namespace LanceNet::net
} // namespace LanceNet

