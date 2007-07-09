#ifndef PutSuccessfulMessage_H_INCLUDED
#define PutSuccessfulMessage_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class PutSuccessfulMessage : public ServerMessage {
  PutSuccessfulMessage() {}
public:
  std::string getIdOfJob() const
  {
    return message->getField("Identifier");
  }

  bool isLast(const std::string &cmd) const
  {
    return true;
  }
  bool isError() const {
      return false;
  }
  friend class ServerMessage;
};

}


#endif // PutSuccessfulMessage_H_INCLUDED
