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

  bool isLast(const std::string &cmd) const
  {
    if (cmd == "ListPeers")
      return false;
    else if (cmd == "ModifyPeer")
      return true;

    throw std::runtime_error("Unknown command");
  }

  bool isError() const {
      return false;
  }

  friend class ServerMessage;
};

}

#endif // PEERMESSAGE_H__
