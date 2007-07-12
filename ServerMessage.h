#ifndef SERVERMESSAGE_H__
#define SERVERMESSAGE_H__

#include "Message.h"
#include "Server.h"
#include <exception>

namespace FCPLib {

class ServerMessage {
  void read(boost::shared_ptr<Server> s);

protected:
  Message::MessagePtr message;

public:
  typedef boost::shared_ptr<ServerMessage> ServerMessagePtr;
  static ServerMessagePtr factory(boost::shared_ptr<Server> s);

  virtual std::string getIdOfJob() const = 0;
  virtual bool isLast(const std::string &cmd) const = 0;
  virtual bool isError() const = 0;
  virtual ~ServerMessage() {}

  const std::string& toString() const;
  const Message::MessagePtr getMessage() const { return message; }
};

}

#endif // SERVERMESSAGE_H__
