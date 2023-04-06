#ifndef LanceNet_BASE_LOGGING_LOGGINGBACKEND_H
#define LanceNet_BASE_LOGGING_LOGGINGBACKEND_H

#include <stdlib.h>
#include <../Thread.h>

namespace LanceNet
{
// flush the buffer to disk file when the buffer is full or every n seconds
//
class LoggerBackend
{
public:
    LoggerBackend(const char* log_file, int flush_per_sec = 3);
    ~LoggerBackend();

    void append(const char* line, size_t size)
    {

    }
private:
    // flush buffer content to file
    void flush();

    int interval_;
    const char* dest;

    // backend file writer thread
    Thread thread;

};

} // namespace LanceNet

#endif // namespace LoggerBackend
