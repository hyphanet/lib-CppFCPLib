#ifndef PERSISTENTREQUESTREMOVEDMESSAGE_H_INCLUDED
#define PERSISTENTREQUESTREMOVEDMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class PersistentRequestRemovedMessage : public ServerMessage {
  PersistentRequestRemovedMessage() {}
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
      return true;
  }
  friend class ServerMessage;
};

}


#endif // PERSISTENTREQUESTREMOVEDMESSAGE_H_INCLUDED
