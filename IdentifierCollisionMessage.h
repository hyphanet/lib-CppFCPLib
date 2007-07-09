#ifndef IDENTIFIERCOLLISIONMESSAGE_H_INCLUDED
#define IDENTIFIERCOLLISIONMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class IdentifierCollisionMessage : public ServerMessage {
  IdentifierCollisionMessage() {}
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

#endif // IDENTIFIERCOLLISIONMESSAGE_H_INCLUDED
