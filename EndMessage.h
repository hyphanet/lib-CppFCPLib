#ifndef ENDMESSAGE_H__
#define ENDMESSAGE_H__

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class EndMessage : public ServerMessage {
  EndMessage() {}
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

#endif // ENDMESSAGE_H__
