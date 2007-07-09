#ifndef STARTEDCOMPRESSIONMESSAGE_H_INCLUDED
#define STARTEDCOMPRESSIONMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class StartedCompressionMessage : public ServerMessage {
  StartedCompressionMessage() {}
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

#endif // STARTEDCOMPRESSIONMESSAGE_H_INCLUDED
