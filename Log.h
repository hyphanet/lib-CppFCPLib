
#ifndef LOG_H_
#define LOG_H_


#include "zthread/Thread.h"
#include "zthread/Mutex.h"
#include <iostream>
#include <string>

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

class Logger;

Logger&
log(std::ostream &out_=std::cerr, verbosityLevel logLevel_=DEBUG);

class Logger{
  friend Logger& log(std::ostream&, verbosityLevel);

  ZThread::Mutex lock;
  std::ostream &out;
  verbosityLevel logLevel;

  Logger(std::ostream &out_, verbosityLevel logLevel_=ERROR);
public:
  void log(verbosityLevel logLevel, const char *message);
  void log(verbosityLevel logLevel, std::string message);
};

}



#endif
