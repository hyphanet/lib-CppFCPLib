#ifndef URIGENERATEDMESSAGE_H_INCLUDED
#define URIGENERATEDMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class URIGeneratedMessage : public ServerMessage {
  URIGeneratedMessage() {}
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
      return false;
  }
  friend class ServerMessage;
};

}

#endif // URIGENERATEDMESSAGE_H_INCLUDED
