#include "Exceptions.h"

using namespace FCPLib;

StdError::StdError() :
	std::runtime_error(strerror(errno)),
	error(errno)
{
}

StdError::StdError(int error_) :
	std::runtime_error(strerror(error_)),
	error(error_)
{
}

StdError::StdError(std::string &func, std::string &message, std::string &errstring) :
  std::runtime_error(func + " : " + message + " " + errstring),
  error(errno)
{
}

StdError::StdError(const char *func, const char *message, const char *errstring) :
  std::runtime_error(std::string(func) + " : " + std::string(message) + " " + std::string(errstring)),
  error(errno)
{
}

StdError::~StdError() throw()
{
}

FCPException::FCPException(Message::MessagePtr m)
  : std::runtime_error(m->getHeader()),
    message(m)
{
}

FCPException::~FCPException() throw()
{
}


