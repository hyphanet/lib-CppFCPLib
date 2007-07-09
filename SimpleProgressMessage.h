#ifndef SIMPLEPROGRESSMESSAGE_H_INCLUDED
#define SIMPLEPROGRESSMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class SimpleProgressMessage : public ServerMessage {
  SimpleProgressMessage() {}
public:
  std::string getIdOfJob() const
  {
    return message->getField("Identifier");
  }

  bool isLast(const std::string &cmd) const
  {
    return false;
  }
  bool isError() const {
      return false;
  }
  friend class ServerMessage;
};

}


#endif // SIMPLEPROGRESSMESSAGE_H_INCLUDED
