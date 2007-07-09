#ifndef NODEHELLOMESSAGE_H__
#define NODEHELLOMESSAGE_H__

namespace FCPLib {

class ServerMessage;

class NodeHelloMessage : public ServerMessage {
  NodeHelloMessage() {}
public:
  std::string getIdOfJob() const
  {
    return "__hello";
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

#endif // NODEHELLOMESSAGE_H__
