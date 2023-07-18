#include "LanceNet/base/ColorText.h"
#include <LanceNet/base/Logging.h>

namespace LanceNet
{

Logger::LogLevel g_loglevel = Logger::LogLevel::TRACE;

// set global log level
Logger::LogLevel setGlobalLogLevel(Logger::LogLevel level)
{
    Logger::LogLevel old = g_loglevel;
    g_loglevel = level;
    return old;
}


const char* Logger::logLevelNames[] =
{
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL"
};
//////////////////////////////
// ctors
//////////////////////////////
// Logger::Logger(CurrentFileName file, int lineNum)
// {
// }

Logger::Logger(CurrentFileName file, int lineNum, LogLevel level, TextColor_t logLevelColor)
  : impl(level, file, lineNum, logLevelColor)
{
}

LogStream& Logger::stream()
{
    return impl.stream();
}

Logger::LogImpl::LogImpl(Logger::LogLevel level, CurrentFileName file, int line, TextColor_t logLevelColor)
  : ts_(system_clock::now()),
    level_(level),
    filename_(file),
    line_(line),
    logLevelColor_(logLevelColor)
{
    stream_ << makeMsgHeader();
}

Logger::LogImpl::~LogImpl()
{
    if(level_ < logLevel())
        return ;
    stream_ << makeMsgTail();
    printf("%s\n", stream_.buffer().c_str());
    if( level_ == Logger::FATAL ){
        // this->abort();
    }
}

LogStream& Logger::LogImpl::stream(){
    return stream_;
}

std::string Logger::LogImpl::makeMsgHeader(){
    char buf[64];
    const char* LogLevelName = Logger::logLevelNames[level_];
    std::string colorLevelName(LogLevelName);
    if(logLevelColor_ != ColorText::NONE_COLOR)
    {
        colorLevelName = ColorText(LogLevelName, logLevelColor_)
                         .asString();
    }

    snprintf(buf, sizeof(buf)-1, "[%5s] %s %05d ", colorLevelName.c_str(),ts_.toFmtString().c_str(), ThisThread::Gettid());
    return buf;
}

std::string Logger::LogImpl::makeMsgTail()
{
    char buf[32];
    snprintf(buf, sizeof(buf)-1, " - %s:%d", filename_.c_str(), line_);
    return std::string(buf);
}

void Logger::LogImpl::abort()
{
   exit(-1);
}

} // namespace LanceNet

