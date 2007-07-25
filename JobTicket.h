#ifndef JOBTICKET_H__
#define JOBTICKET_H__

#include "Message.h"
#include "Exceptions.h"
#include "ServerMessage.h"

#include <vector>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "zthread/Guard.h"
#include "zthread/FastMutex.h"
#include "zthread/Mutex.h"

#include <boost/function.hpp>

namespace FCPLib {

class NodeThread;
class Node;

class JobTicket {
public:
  typedef boost::shared_ptr<JobTicket> Ptr;
private:
  std::string id;
  Message::Ptr cmd;

  Response nodeResponse;

  bool keep;
  bool global;
  bool persistent;

  std::string repr;
  bool isReprValid;

  bool _isFinished;

  ZThread::Mutex access;

  ZThread::FastMutex lock;  // used to be able to wait until isFinished
  ZThread::FastMutex reqSentLock; // wait until a confirm message is received
  int timeQueued;

  boost::function<void (int, const ServerMessage::Ptr)> f;

  void putResult();
  // status... last message -- 0, not last message -- 1
  void putResponse(int status, ServerMessage::Ptr m)
  {
    ZThread::Guard<ZThread::Mutex> g(access);
    if (f) f(status, m);
    if (nodeResponse.empty())
      reqSentLock.release(); // first message has arrived, so it has been successfully submitted
    nodeResponse.push_back(m);
  }

  JobTicket()
    : keep(false),
      global(false),
      persistent(false),
      isReprValid(false),
      _isFinished(false)
  {}
  static Ptr factory(std::string id, Message::Ptr cmd);

  JobTicket& setKeep( bool x ) { keep = x; return *this; };
  JobTicket& setGlobal( bool x ) { global = x; return *this; };
  JobTicket& setPersistent( bool x ) { persistent = x; return *this; };
  void setCallback( boost::function<void (int, const ServerMessage::Ptr)> f )
  {
    this->f = f;
  }
  void setCallback( void (*f)(int, const ServerMessage::Ptr) )
  {
    this->f = f;
  }
public:
  const std::string& getCommandName() const;
  const std::string& getId() const;
  const Message::Ptr getCommand() const;

  void wait(unsigned int timeout_=0);
  void waitTillReqSent(unsigned int timeout);

  Response getResponse()
  {
    ZThread::Guard<ZThread::Mutex> g(access);
    return Response( nodeResponse );
  }
  const std::string& toString();

  bool isGlobal() const { return global; }
  bool isPersistent() const { return persistent; }

  bool isFinished()
  {
    ZThread::Guard<ZThread::Mutex> g(access);
    return _isFinished;
  }

  friend class Node;
  friend class NodeThread;
};

typedef std::vector<JobTicket::Ptr> JobCollection;

}

#endif
