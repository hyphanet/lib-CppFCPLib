#ifndef PEERMESSAGE_H__
#define PEERMESSAGE_H__

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class PeerMessage : public ServerMessage {
  PeerMessage() {}
public:
  std::string getIdOfJob() const
  {
    return "__global";
  }

  bool isLastMessage(const std::string &cmd) const
  {
    if (cmd == "ListPeers")
      return false;
    else if (cmd == "ModifyPeer")
      return true;

    throw new std::runtime_error("Unknown command");
  }

  friend class ServerMessage;
};

}

#endif // PEERMESSAGE_H__
