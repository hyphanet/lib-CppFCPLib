#ifndef JOBTICKET_H__
#define JOBTICKET_H__

#include "Message.h"
#include "ServerMessage.h"

#include <stdexcept>
#include <vector>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "zthread/Guard.h"
#include "zthread/FastMutex.h"
#include "zthread/Mutex.h"

#include <boost/function.hpp>

namespace FCPLib {

class NodeThread;

class Response {
public:
  Response( std::vector<ServerMessage::Ptr> &r) : responses(r) {}

  std::vector<ServerMessage::Ptr> responses;
};

class JobTicket {
  std::string id;
  Message::Ptr cmd;

  std::vector<ServerMessage::Ptr> nodeResponse;

  bool keep;

  std::string repr;
  bool isReprValid;

  bool _isFinished;

  ZThread::Mutex access;

  ZThread::FastMutex lock;  // used to be able to wait until isFinished
  ZThread::FastMutex reqSentLock;
  int timeQueued;

  boost::function<void (int, const ServerMessage::Ptr)> f;

  void putResult();
  // status... last message -- 0, not last message -- 1
  void putResponse(int status, ServerMessage::Ptr m)
  {
    ZThread::Guard<ZThread::Mutex> g(access);
    if (f) f(status, m);
    nodeResponse.push_back(m);
  }

  JobTicket() : isReprValid(false), _isFinished(false) {}
public:
  typedef boost::shared_ptr<JobTicket > Ptr;

  static Ptr factory(std::string id, Message::Ptr cmd, bool keep);

  void setCallback( boost::function<void (int, const ServerMessage::Ptr)> f )
  {
    this->f = f;
  }

  void setCallback( void (*f)(int, const ServerMessage::Ptr) )
  {
    this->f = f;
  }

  const std::string& getCommandName() const;
  const std::string& getId() const;
  const std::string& getMessageText() const;
  const Message::Ptr getCommand() const;

  void wait(unsigned int timeout_=0);
  void waitTillReqSent();

  Response getResponse()
  {
    ZThread::Guard<ZThread::Mutex> g(access);
    return Response( nodeResponse );
  }
  const std::string& toString();

  bool isFinished()
  {
    ZThread::Guard<ZThread::Mutex> g(access);
    return _isFinished;
  }

  friend class NodeThread;
};


}

#endif
