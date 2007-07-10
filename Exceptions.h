
#ifndef EXCEPTIONS_H__
#define EXCEPTIONS_H__

#include <stdexcept>
#include <cerrno>
#include "Message.h"

namespace FCPLib {

class StdError : public std::runtime_error {
  int error;
public:
  StdError();
  StdError(int error_);
  StdError(std::string &func, std::string &message, std::string &errstring);
  StdError(const char *func, const char *message, const char *errstring);
  ~StdError() throw();
};

class FCPException : public std::runtime_error {
  Message::MessagePtr message;
public:
  FCPException(std::string msg);
  FCPException(Message::MessagePtr m);
  const Message::MessagePtr& getMessage() const {
    return message;
  }
  ~FCPException() throw();
};

}

#endif
