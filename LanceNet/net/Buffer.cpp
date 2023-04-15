#include <LanceNet/net/Buffer.h>
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

Buffer::Buffer(int initSz, int prependSz)
  : buf_(initSz + prependSz),
    readindex_(prependSz),
    writeindex_(prependSz)
{
}

size_t Buffer::readableBytes() const
{
    return writeindex_ - readindex_;
}

size_t Buffer::writeableBytes() const
{
    return buf_.size() - writeindex_;
}

size_t Buffer::prependableBytes() const
{
    return readindex_;
}

////////////////////////////////////////
/// peeks
////////////////////////////////////////
const char* Buffer::peek() const
{
    assert(readindex_ < buf_.size() && readindex_ <= writeindex_);
    return beginRead();
}

int8_t Buffer::peekInt8() const
{
    assert(readableBytes() >= sizeof(int8_t));
    const int8_t* p = reinterpret_cast<const int8_t*>(peek());
    return *p;
}

int16_t Buffer::peekInt16() const
{
    assert(readableBytes() >= sizeof(int16_t));
    const int16_t* pint = reinterpret_cast<const int16_t*>(peek());
    return *pint;
}

int32_t Buffer::peekInt32() const
{
    assert(readableBytes() >= sizeof(int32_t));
    const int32_t* pint = reinterpret_cast<const int32_t*>(peek());
    return *pint;
}

int64_t Buffer::peekInt64() const
{
    assert(readableBytes() >= sizeof(int64_t));
    const int64_t* pint = reinterpret_cast<const int64_t*>(peek());
    return *pint;
}

/////////////////////////////////////////////////////////
/// retrieve
/////////////////////////////////////////////////////////
void Buffer::retrieve(size_t size)
{
    assert(size >= 0 && size <= readableBytes());
    hasRead(size);

    // reset to begin (done at `append()`)
    // if(readindex_ == writeindex_)
    // {
    //     readindex_ = writeindex_ = kDefaultPrependSize;
    // }
}

void Buffer::retrieveAll()
{
    // reset watermark
    readindex_ = writeindex_ = kDefaultPrependSize;
}

void Buffer::retrieveInt8()
{
    retrieve(sizeof(int8_t));
}

void Buffer::retrieveInt16()
{
    retrieve(sizeof(int16_t));
}

void Buffer::retrieveInt32()
{
    retrieve(sizeof(int32_t));
}

void Buffer::retrieveInt64()
{
    retrieve(sizeof(int64_t));
}

std::string Buffer::retrieveAllAsString()
{
    std::string rv = std::string(buf_.begin() + readindex_, buf_.begin() + writeindex_);
    retrieve(readableBytes());

    readindex_ = writeindex_ = kDefaultPrependSize;
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


////////////////////////////////////////////////////////
// append
////////////////////////////////////////////////////////

void Buffer::append(const void* src, size_t len)
{
    append(static_cast<const char*>(src), len);
}

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
    if(writeableBytes() + prependableBytes() >= len + kDefaultPrependSize){
        size_t readable = readableBytes();

        readindex_ = kDefaultPrependSize;
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
//        writeable + prependable >= len + kDefaultPrependSize)
//     {
//         assert(readindex_ <= writeindex_ && writeindex_ <= buf_.size());
//         // step 1: move the buffer to the begining of buf_
//         memmove(&buf_[kDefaultInitSize], beginRead(), readableBytes());
//         // step 2: update watermark
//         int offset = readindex_ - kDefaultPrependSize;
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

void Buffer::append(const StringPiece& str)
{
    const char* cstr = str.data();
    this->append(cstr, str.size());
}

void Buffer::appendInt8(int8_t num)
{
    append(&num, sizeof(int8_t));
}

void Buffer::appendInt16(int16_t num)
{
    append(&num, sizeof(int16_t));
}

void Buffer::appendInt32(int32_t num)
{
    append(&num, sizeof(int32_t));
}

void Buffer::preappend(const void * src, size_t len)
{
    assert(len <= prependableBytes());
    assert(beginRead() - len >= begin());
    memcpy(beginRead() - len, src, len);
    unread(len);
}
void Buffer::preappend(int64_t num)
{
    preappend(&num, sizeof(int64_t));
}

void Buffer::preappend(int32_t num)
{
    preappend(&num, sizeof(int32_t));
}

void Buffer::preappend(int16_t num)
{
    preappend(&num, sizeof(int16_t));
}

void Buffer::preappend(int8_t num)
{
    preappend(&num, sizeof(int8_t));
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

/////////////////////////////////////////
/// privates
/////////////////////////////////////////
char* Buffer::begin()
{
    return &(*buf_.begin());
} // namespace net

const char* Buffer::begin() const
{
    return &(*buf_.begin());
}

char* Buffer::beginRead()
{
    return begin() + readindex_;
}
const char* Buffer::beginRead() const
{
    return begin() + readindex_;
}

char* Buffer::beginWrite()
{
    return begin() + writeindex_;
}
const char* Buffer::beginWrite() const
{
    return begin() + writeindex_;
}

void Buffer::hasRead(size_t sz)
{
    readindex_ += sz;
}
void Buffer::hasWritten(size_t sz)
{
    writeindex_ += sz;
}
void Buffer::unread(size_t sz)
{
    readindex_ -= sz;
}

} // namespace LanceNet::net
} // namespace LanceNet

