
#include "Log.h"
#include "Exceptions.h"
#include "zthread/Guard.h"

using namespace FCPLib;

Logger::Logger(std::ostream &out_, verbosityLevel logLevel_)
  : out(out_),
    logLevel(logLevel_)
{
}

void
Logger::log(verbosityLevel logLevel_, const char *message)
{
  ZThread::Guard<ZThread::Mutex> g(lock);

  if (logLevel_ <= logLevel) {
      out << message;
      if (message[strlen(message)] != '\n')
        out << '\n';
  }
  out.flush();
}

void
Logger::log(verbosityLevel logLevel_, std::string message)
{
  log(logLevel_, message.c_str());
}

Logger&
FCPLib::log(std::ostream &out_, verbosityLevel logLevel_)
{
  static bool firstTime = true;
  static Logger* l;

  if (firstTime) {
    firstTime = false;
    l = new Logger(out_, logLevel_);
  }

  return *l;
}

