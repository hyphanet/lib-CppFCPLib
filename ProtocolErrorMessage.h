#ifndef PROTOCOLERRORMESSAGE_H_INCLUDED
#define PROTOCOLERRORMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class ProtocolErrorMessage : public ServerMessage {
  ProtocolErrorMessage() {}
public:
  std::string getIdOfJob() const
  {
    const std::string& ret = message->getField("Identifier");
    return  ret.size() ? ret : "__global";
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

#endif // PROTOCOLERRORMESSAGE_H_INCLUDED
