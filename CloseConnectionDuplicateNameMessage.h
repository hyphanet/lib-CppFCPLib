#ifndef CLOSECONNECTIONDUPLICATENAMEMESSAGE_H_INCLUDED
#define CLOSECONNECTIONDUPLICATENAMEMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class CloseConnectionDuplicateNameMessage : public ServerMessage {
  CloseConnectionDuplicateNameMessage() {}
public:
  std::string getIdOfJob() const
  {
    return "__hello";
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


#endif // CLOSECONNECTIONDUPLICATENAMEMESSAGE_H_INCLUDED
