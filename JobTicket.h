#ifndef JOBTICKET_H__
#define JOBTICKET_H__

#include "Message.h"
#include "ServerMessage.h"
#include "FCPResult.h"

#include <stdexcept>
#include <vector>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "zthread/FastMutex.h"

namespace FCPLib {

class NodeThread;


class JobTicket {
  std::string id;
  Message::MessagePtr cmd;

  std::vector<ServerMessage::ServerMessagePtr> nodeResponse;

  bool async;
  bool keep;
  bool waitTillSent;
  int timeout;

  std::string repr;
  bool isReprValid;

  FCPResult::FCPResultPtr result;
  bool _hasResult;

//  bool hasStream;
//  ostream stream;

  ZThread::FastMutex lock;
  ZThread::FastMutex reqSentLock;
  int timeQueued;

  void putResult();


  JobTicket()
  {
    _hasResult = false;
    isReprValid = false;
  }
public:
  typedef boost::shared_ptr<JobTicket > JobTicketPtr;

  static
  JobTicketPtr factory(std::string id, Message::MessagePtr cmd,
                             bool async, bool keep, bool waitTillSent,
                             int timeout);

  const std::string& getCommandName() const;
  const std::string& getId() const;
  const std::string& getMessageText() const;
  const Message::MessagePtr getCommand() const;

  void wait(unsigned int timeout_=0);
  void waitTillReqSent();

  const std::vector<ServerMessage::ServerMessagePtr>& getResponse() const;
  const std::string& toString();

  const FCPResult::FCPResultPtr getResult() const;
  bool hasResult() const;

  friend class NodeThread;
};



}

#endif
