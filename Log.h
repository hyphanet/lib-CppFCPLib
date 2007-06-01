
#ifndef LOG_H_
#define LOG_H_


#include "zthread/Thread.h"
#include "zthread/Mutex.h"
#include <iostream>
#include <string>

using namespace std;

namespace FCPLib {

typedef enum {
    SILENT,
    FATAL,
    CRITICAL,
    ERROR,
    INFO,
    DETAIL,
    DEBUG,
    NOISY,
} verbosityLevel;

class Logger{
  ZThread::Mutex lock;
  ostream &out;
  verbosityLevel logLevel;
public:
    Logger(ostream &out_, verbosityLevel logLevel_=ERROR);
    void log(verbosityLevel logLevel, const char *message);
    void log(verbosityLevel logLevel, string message);
};

}

FCPLib::Logger& log(ostream &out_=cerr, FCPLib::verbosityLevel logLevel_=FCPLib::NOISY);

#endif
