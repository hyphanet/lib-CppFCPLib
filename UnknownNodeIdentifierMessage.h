#ifndef UNKNOWNNODEIDENTIFIERMESSAGE_H_INCLUDED
#define UNKNOWNNODEIDENTIFIERMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class UnknownNodeIdentifierMessage : public ServerMessage {
  UnknownNodeIdentifierMessage() {}
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
      return true;
  }
  friend class ServerMessage;
};

}

#endif // UNKNOWNNODEIDENTIFIERMESSAGE_H_INCLUDED
