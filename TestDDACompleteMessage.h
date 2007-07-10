#ifndef TESTDDACOMPLETEMESSAGE_H_INCLUDED
#define TESTDDACOMPLETEMESSAGE_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {

class ServerMessage;

class TestDDACompleteMessage : public ServerMessage {
  TestDDACompleteMessage() {}
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

#endif // TESTDDACOMPLETEMESSAGE_H_INCLUDED
