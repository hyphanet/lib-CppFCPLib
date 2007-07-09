#ifndef CONFIGDATAMESSAGE_H_INCLUDED
#define CONFIGDATAMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class ConfigDataMessage : public ServerMessage {
  ConfigDataMessage() {}
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

#endif // CONFIGDATAMESSAGE_H_INCLUDED
