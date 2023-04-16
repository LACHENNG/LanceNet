
#include <LanceNet/base/LogStream.h>
#include <LanceNet/base/num2cstr.h>

namespace LanceNet
{

LogStream& LogStream::operator<<(bool data)
{
    if(data) this->append("True");
    else this->append("False");
    return *this;
}

LogStream& LogStream::operator<<(short data)
{
    char buf[16]; 
    this->append(itoa_s(buf, data));
    return *this;
}

LogStream& LogStream::operator<<(unsigned short data)
{
    char buf[16]; 
    this->append(itoa_s(buf, data));
    return *this;
}

LogStream& LogStream::operator<<(int data)
{
    char buf[16]; 
    this->append(itoa_s(buf, data));
    return *this;
}

LogStream& LogStream::operator<<(unsigned int data)
{
    char buf[16]; 
    this->append(itoa_s(buf, data));
    return *this;
}

LogStream& LogStream::operator<<(long data)
{
    char buf[16]; 
    this->append(itoa_s(buf, data));
    return *this;
}

LogStream& LogStream::operator<<(unsigned long data)
{
    char buf[32]; 
    this->append(itoa_s(buf, data));
    return *this;
}

LogStream& LogStream::operator<<(long long data)
{
    char buf[32]; 
    this->append(itoa_s(buf, data));
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long data)
{
    char buf[32]; 
    this->append(itoa_s(buf, data));
    return *this;
}

// TODO: optimize float to str conversion

LogStream& LogStream::operator<<(float data)
{
    char buf[32];
    snprintf(buf, 32, "%f", data);
    this->append(buf);
    return *this;
}

LogStream& LogStream::operator<<(double data)
{
    char buf[32];
    snprintf(buf, 32, "%lf", data);
    this->append(buf);
    return *this;
}

LogStream& LogStream::operator<<(const void *addr)
{
    char buf[16];
    std::snprintf(buf, 16, "%p", addr);
    this->append(buf);
    return *this;
}

LogStream& LogStream::operator<<(const char* pstr)
{
    this->append(pstr);
    return *this;
}

LogStream& LogStream::operator<<(const std::string& str)
{
    this->append(str.c_str());
    return *this;
}

void LogStream::append(const char* line)
{
    buf_.append(line);
}

void LogStream::resetBuf()
{
    buf_.clear();
}

} // namespace LanceNet
