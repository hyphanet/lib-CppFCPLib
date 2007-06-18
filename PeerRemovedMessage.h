#ifndef PEERREMOVEDMESSAGE_H__
#define PEERREMOVEDMESSAGE_H__

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class PeerRemovedMessage : public ServerMessage {
  PeerRemovedMessage() {}
public:
  std::string getIdOfJob() const
  {
    return "__global";
  }

  bool isLastMessage(const std::string &cmd) const
  {
   return true;
  }

  friend class ServerMessage;
};

}

#endif // PEERREMOVEDMESSAGE_H__
