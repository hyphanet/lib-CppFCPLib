#ifndef PEERNOTEMESSAGE_H__
#define PEERNOTEMESSAGE_H__

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class PeerNoteMessage : public ServerMessage {
  PeerNoteMessage() {}
public:
  std::string getIdOfJob() const
  {
    return "__global";
  }

  bool isLastMessage(const std::string &cmd) const
  {
    if (cmd == "ListPeerNotes")
      return false;
    else if (cmd == "ModifyPeerNote")
      return true;

    throw new std::runtime_error("Unknown command");
  }

  friend class ServerMessage;
};

}
#endif // PEERNOTEMESSAGE_H__
