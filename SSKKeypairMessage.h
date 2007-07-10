#ifndef SSKKEYPAIRMESSAGE_H_INCLUDED
#define SSKKEYPAIRMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class SSKKeypairMessage : public ServerMessage {
  SSKKeypairMessage() {}
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

#endif // SSKKEYPAIRMESSAGE_H_INCLUDED
