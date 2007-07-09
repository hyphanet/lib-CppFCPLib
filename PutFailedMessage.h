#ifndef PUTFAILEDMESSAGE_H_INCLUDED
#define PUTFAILEDMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class PutFailedMessage : public ServerMessage {
  PutFailedMessage() {}
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
      return true;
  }
  friend class ServerMessage;
};

}

#endif // PUTFAILEDMESSAGE_H_INCLUDED
