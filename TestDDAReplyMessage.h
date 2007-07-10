#ifndef TESTDDAREPLYMESSAGE_H_INCLUDED
#define TESTDDAREPLYMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class TestDDAReplyMessage : public ServerMessage {
  TestDDAReplyMessage() {}
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

#endif // TESTDDAREPLYMESSAGE_H_INCLUDED
