#ifndef GETFAILEDMESSAGE_H_INCLUDED
#define GETFAILEDMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class GetFailedMessage : public ServerMessage {
  GetFailedMessage() {}
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

#endif // GETFAILEDMESSAGE_H_INCLUDED
