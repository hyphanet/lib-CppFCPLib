#ifndef NODETHREAD_H__
#define NODETHREAD_H__

#include <zthread/Thread.h>
#include <map>
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "JobTicket.h"
#include "ServerMessage.h"
#include "TQueue.h"
#include "Log.h"
#include "Server.h"


namespace FCPLib {

class Node;

typedef TQueue< JobTicket::Ptr > JobTicketQueue;

class NodeThread : public ZThread::Runnable {
  ZThread::CountedPtr< JobTicketQueue > clientReqQueue;
  std::string host_;
  int port_;
  boost::shared_ptr<Server> s;

  bool isAlive_;
  ZThread::CountedPtr<std::exception> exception;

  ZThread::CountedPtr< std::map<std::string, JobTicket::Ptr > > jobs;

  friend class Node;
  NodeThread(std::string &host, int port, ZThread::CountedPtr< JobTicketQueue > clientReqQueue_) throw();

  void sendClientReq(JobTicket::Ptr job);
  void doMessage(ServerMessage::Ptr message);
public:
  void run();
  bool isAlive() const {
    return isAlive_;
  }
  ZThread::CountedPtr<std::exception> getFailure() const {
    return exception;
  }
};

}
#endif

