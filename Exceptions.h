
#ifndef EXCEPTIONS_H__
#define EXCEPTIONS_H__

#include <stdexcept>
#include <cerrno>

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

}

#endif
