
#ifndef EXCEPTIONS_H__
#define EXCEPTIONS_H__

#include <stdexcept>
#include <cerrno>
#include "Message.h"

namespace FCPLib {

class FCPException : public std::runtime_error {
  Message::Ptr message;
public:
  FCPException(Message::Ptr m);
  const Message::Ptr getMessage() const {
    return message;
  }
  ~FCPException() throw();
};

class FCPNodeFailure : public std::runtime_error {
public:
  FCPNodeFailure(std::string msg) : std::runtime_error(msg) {}
};

class FileError : public std::runtime_error {
  std::string path_;
public:
  FileError( std::string msg, std::string path )
    : std::runtime_error(msg),
      path_(path) {}
  const std::string& getFile() const { return path_; }
  ~FileError() throw() {}
};

class NotImplemented : public std::logic_error {
public:
  NotImplemented(std::string msg) : std::logic_error(msg) {}
};

class CommandTimeout : public std::runtime_error {
public:
  CommandTimeout(std::string msg) : std::runtime_error(msg) {}
};

class ConnectionRefused : public std::runtime_error {
public:
  ConnectionRefused(std::string msg) : std::runtime_error(msg) {}
};

class TestDDAError : public std::runtime_error {
public:
  TestDDAError(std::string msg) : std::runtime_error(msg) {}
};


}

#endif
