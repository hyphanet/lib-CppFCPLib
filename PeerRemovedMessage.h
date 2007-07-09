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

#endif // PEERREMOVEDMESSAGE_H__
