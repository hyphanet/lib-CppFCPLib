#ifndef FINISHEDCOMPRESSIONMESSAGE_H_INCLUDED
#define FINISHEDCOMPRESSIONMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class FinishedCompressionMessage : public ServerMessage {
  FinishedCompressionMessage() {}
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

#endif // FINISHEDCOMPRESSIONMESSAGE_H_INCLUDED
