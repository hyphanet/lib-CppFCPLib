#ifndef NODEDATAMESSAGE_H__
#define NODEDATAMESSAGE_H__

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class NodeDataMessage : public ServerMessage {
  NodeDataMessage() {}
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

#endif // NODEDATAMESSAGE_H__
