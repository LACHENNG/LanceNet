// Author : lance @ npuw 
// this is a simpe usage and tests of LOG functionalities
//
#include "../Time.h"
#include "iostream"
#include "LanceNet/base/Logging.h"
#include "../ColorText.h"

using namespace LanceNet;

using namespace std;

int main()
{
    Logger::setLogLevel(Logger::LogLevel::TRACE);
    cout << "Set global log level to " << Logger::logLevelNames[Logger::logLevel()] << endl ;
    LOG_TRACE << "Trace" ;
    LOG_DEBUG << "Debug";
    LOG_INFO << "INFO";
    LOG_WARN << "WARN" ;
    LOG_ERROR << "ERROR";
    LOG_FATAL << ColorText("FATAL", ColorText::TextColor::RED);
    LOG_TRACEC << "TraceC" ;
    LOG_DEBUGC << "DebugC";
    LOG_INFOC << "INFOC";
    LOG_WARNC << "WARNC" ;
    LOG_ERRORC << "LOG_ERRORC";
    LOG_FATALC << "FATALC" ;


    Logger::setLogLevel(Logger::LogLevel::DEBUG);
    cout << "\n Set global log level to " << Logger::logLevelNames[Logger::logLevel()] << endl ;
    LOG_TRACE << "Trace" ;
    LOG_DEBUG << "Debug";
    LOG_INFO << "INFO";
    LOG_WARN << "WARN" ;
    LOG_ERROR << "ERROR";
    LOG_FATAL << "FATAL" ;
    LOG_TRACEC << "TraceC" ;
    LOG_DEBUGC << "DebugC";
    LOG_INFOC << "INFOC";
    LOG_WARNC << "WARNC" ;
    LOG_ERRORC << "LOG_ERRORC";
    LOG_FATALC << "FATALC" ;

    Logger::setLogLevel(Logger::LogLevel::INFO);
    cout << "\n Set global log level to " << Logger::logLevelNames[Logger::logLevel()] << endl ;
    LOG_TRACE << "Trace" ;
    LOG_DEBUG << "Debug";
    LOG_INFO << "INFO";
    LOG_WARN << "WARN" ;
    LOG_ERROR << "ERROR";
    LOG_FATAL << "FATAL" ;
    LOG_TRACEC << "TraceC" ;
    LOG_DEBUGC << "DebugC";
    LOG_INFOC << "INFOC";
    LOG_WARNC << "WARNC" ;
    LOG_ERRORC << "LOG_ERRORC";
    LOG_FATALC << "FATALC" ;
    
    Logger::setLogLevel(Logger::LogLevel::WARN);
    cout << "\n Set global log level to " << Logger::logLevelNames[Logger::logLevel()] << endl ;
    LOG_TRACE << "Trace" ;
    LOG_DEBUG << "Debug";
    LOG_INFO << "INFO";
    LOG_WARN << "WARN" ;
    LOG_ERROR << "ERROR";
    LOG_FATAL << "FATAL" ;
    LOG_TRACEC << "TraceC" ;
    LOG_DEBUGC << "DebugC";
    LOG_INFOC << "INFOC";
    LOG_WARNC << "WARNC" ;
    LOG_ERRORC << "LOG_ERRORC";
    LOG_FATALC << "FATALC" ;

    Logger::setLogLevel(Logger::LogLevel::ERROR);
    cout << "\n Set global log level to " << Logger::logLevelNames[Logger::logLevel()] << endl ;
    LOG_TRACE << "Trace" ;
    LOG_DEBUG << "Debug";
    LOG_INFO << "INFO";
    LOG_WARN << "WARN" ;
    LOG_ERROR << "ERROR";
    LOG_FATAL << "FATAL" ;
    LOG_TRACEC << "TraceC" ;
    LOG_DEBUGC << "DebugC";
    LOG_INFOC << "INFOC";
    LOG_WARNC << "WARNC" ;
    LOG_ERRORC << "LOG_ERRORC";
    LOG_FATALC << "FATALC" ;


    Logger::setLogLevel(Logger::LogLevel::FATAL);
    cout << "\n Set global log level to " << Logger::logLevelNames[Logger::logLevel()] << endl ;
    LOG_TRACE << "Trace" ;
    LOG_DEBUG << "Debug";
    LOG_INFO << "INFO";
    LOG_WARN << "WARN" ;
    LOG_ERROR << "ERROR";
    LOG_FATAL << "FATAL" ;
    LOG_TRACEC << "TraceC";
    LOG_DEBUGC << "DebugC";
    LOG_INFOC << "INFOC";
    LOG_WARNC << "WARNC" ;
    LOG_ERRORC << "LOG_ERRORC";
    LOG_FATALC << "FATALC" ;
    return 0;
}
