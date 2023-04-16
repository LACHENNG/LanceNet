// Author : Lance(Lang Chen) @ nwpu
// frontend, user visible include file
#ifndef LanceNet_BASE_LOGGING_LOGGING_H
#define LanceNet_BASE_LOGGING_LOGGING_H

#include <iostream>
#include <functional>

#include <LanceNet/base/Time.h>
#include <LanceNet/base/ThisThread.h>
#include <LanceNet/base/noncopyable.h>
#include <LanceNet/base/LogStream.h>
#include <LanceNet/base/ColorText.h>

#ifndef __GNUC__
#warning "No GNUC Support, using strrchr instead of __builtin_strrchr"
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

// log format example [LOG_LEVEL] 20230101 12:30:00.1234431 tid Message - File:Line
class Logger : noncopyable{
public:
    using LogCallback = std::function<void(LogStream& stream)>;

    // log level definitions
    enum LogLevel{
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };
    static const char* logLevelNames[LogLevel::NUM_LOG_LEVELS];

    static Logger::LogLevel logLevel();
    static void setLogLevel(Logger::LogLevel level);

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
    using TextColor_t = ColorText::TextColor;
    // ctors
    // Logger(CurrentFileName file, int lineNum);
    Logger(CurrentFileName file, int lineNum, LogLevel level, TextColor_t color = ColorText::NONE_COLOR);
    // Logger(CurrentFileName file, int lineNum, LogCallback cb);
    // Logger(CurrentFileName file, int lineNum, bool toAbort);

    LogStream& stream();

private:
    // the nested class is mainly used to reduce the implementation complexity
    // it actually buffer the log message
    class LogImpl{
    public:
        LogImpl(LogLevel level, CurrentFileName file, int line, TextColor_t logLevelColor);
        ~LogImpl();

        LogStream& stream();
    private:
        std::string makeMsgHeader();
        std::string makeMsgTail();
        void abort();
    private:
        TimeStamp ts_;
        LogStream stream_;
        LogLevel level_;
        CurrentFileName filename_;
        int line_;
        TextColor_t logLevelColor_; 
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

// non color version
#define LOG_WARN  LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::WARN).stream()
#define LOG_INFO  LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::INFO).stream()
#define LOG_FATAL LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::FATAL).stream()

// color versoin
#define LOG_WARNC  LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::WARN, LanceNet::ColorText::YELLOW).stream()
#define LOG_INFOC  LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::INFO, LanceNet::ColorText::GREEN).stream()
#define LOG_FATALC LanceNet::Logger(BASENAME(__FILE__), __LINE__, LanceNet::Logger::LogLevel::FATAL, LanceNet::ColorText::RED).stream()



#endif // LanceNet_BASE_LOGGING_LOGGING_H
