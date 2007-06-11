#ifndef SERVERMESSAGE_H__
#define SERVERMESSAGE_H__

#include "Message.h"
#include "Server.h"

namespace FCPLib {

class ServerMessage {
  Message::MessagePtr message;

  void read(Server &s);
public:
  typedef boost::shared_ptr<ServerMessage> ServerMessagePtr;
  static ServerMessagePtr factory(Server &s);

  virtual std::string getIdOfJob() const = 0;
  virtual bool isLastMessage(const std::string &cmd) const = 0;
  virtual ~ServerMessage() {}
};

}

#endif // SERVERMESSAGE_H__
