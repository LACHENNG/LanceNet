// Author : Lance(Lang Chen) @ nwpu
// frontend
#ifndef LanceNet_BASE_LOGGING_LOGGING_H
#define LanceNet_BASE_LOGGING_LOGGING_H

#include "LogStream.h"
#include <functional>
#include "Time.h"
#include "../ThisThread.h"

#include "iostream"
// #include <utility>

#ifndef __GNUC__
    #include <string.h>
    #define __builtin_strrchr strrchr
#endif

// calculate the basename of a file at compile time
#define BASENAME(FILE) (__builtin_strrchr(FILE, '/') ? __builtin_strrchr(FILE, '/') + 1 : FILE)

namespace LanceNet
{

// Logger class that optimized to get current file name at complie time (But Only under GCC Support)

// There is a global defualt LogLevel which will influence the manner of Logger
// specifically, when Logger.loglevel <= g_loglevel, the log will be ignored

// log format example 20230101 12:30:00.1234431 TID LEVEL Message - File:Line
class Logger : noncopyable{
public:
    using LogCallback = std::function<void(LogStream& stream)>;

    // log level definitions
    enum LogLevel{
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    // a possibly GCC optimized compile time file basename acquistion
    class CurrentFileName{
    public:
        CurrentFileName(const char* file_basename) : filename(file_basename)
        {
        }
        const char* c_str() const { return filename; }
    private:
        const char* filename;
    };


public:
    // ctors
    // Logger(CurrentFileName file, int lineNum);
    Logger(CurrentFileName file, int lineNum, LogLevel level);
    // Logger(CurrentFileName file, int lineNum, LogCallback cb);
    // Logger(CurrentFileName file, int lineNum, bool toAbort);

    // stream style usage
    LogStream& stream();

private:
    // the nested class is mainly used to reduce the implementation complexity
    // it actually buffer the log message
    class LogImpl{
    public:

        LogImpl(LogLevel level, CurrentFileName file, int line)
          : ts_(system_clock::now()),
            level_(level),
            filename_(file),
            line_(line)
        {
            stream_ << makeMesHeader();
        }

        ~LogImpl()
        {
            char buf[32];
            snprintf(buf, sizeof(buf)-1, " - %s:%d", filename_.c_str(), line_);
            stream_ << buf;
            std::cout << stream_.buffer() << std::endl;
        }

        LogStream& stream(){
            return stream_;
        }
        std::string makeMesHeader(){
            char buf[64];
            snprintf(buf, sizeof(buf)-1, "%s %05d ", ts_.toFmtString().c_str(), ThisThread::Gettid());
            return buf;
        }
    private:
        TimeStamp ts_;
        LogStream stream_;
        LogLevel level_;
        CurrentFileName filename_;
        int line_;
    };

    LogImpl impl;
};


// global log level
extern Logger::LogLevel g_loglevel;

inline Logger::LogLevel Logger::logLevel()
{
    return g_loglevel;
}

inline void Logger::setLogLevel(Logger::LogLevel level)
{
    g_loglevel = level;
}

Logger::LogLevel setGlobalLogLevel(Logger::LogLevel level);



} // namespace LanceNet

#define LOG_WARN  LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::WARN).stream()
#define LOG_INFO  LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::INFO).stream()
#define LOG_FATAL LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::FATAL).stream()


#endif // LanceNet_BASE_LOGGING_LOGGING_H
