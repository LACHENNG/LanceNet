#include "Logging.h"

namespace LanceNet
{

Logger::LogLevel g_loglevel = Logger::LogLevel::INFO;

// set global log level
Logger::LogLevel setGlobalLogLevel(Logger::LogLevel level)
{
    Logger::LogLevel old = g_loglevel;
    g_loglevel = level;
    return old;
}

//////////////////////////////
// ctors
//////////////////////////////
// Logger::Logger(CurrentFileName file, int lineNum)
// {
// }

Logger::Logger(CurrentFileName file, int lineNum, LogLevel level)
  : impl(level, file, lineNum)
{
}

LogStream& Logger::stream()
{
    // appending log header before return the logstream to user
    // such that the user message will automatically carry more 
    // info such as timestamp
    return impl.stream();
}

} // namespace LanceNet

