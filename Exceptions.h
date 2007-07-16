
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

}

#endif
