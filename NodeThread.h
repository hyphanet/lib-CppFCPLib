#ifndef NODETHREAD_H__
#define NODETHREAD_H__

#include "zthread/Thread.h"
#include "TQueue.h"
#include "Log.h"
#include "Server.h"
#include <map>
#include <string>
#include "JobTicket.h"
#include "ServerMessage.h"

namespace FCPLib {

class Node;

typedef TQueue< JobTicket::JobTicketPtr > JobTicketQueue;

class NodeThread : public ZThread::Runnable {
  ZThread::CountedPtr< JobTicketQueue > clientReqQueue;
  FCPLib::Server s;

  ZThread::CountedPtr< std::map<std::string, JobTicket::JobTicketPtr > > jobs;

  friend class Node;
  NodeThread(std::string &host, int port, ZThread::CountedPtr< JobTicketQueue > &clientReqQueue_) throw();

  void sendClientReq(JobTicket::JobTicketPtr &job);
  void doMessage(ServerMessage::ServerMessagePtr &message);
public:
  void run();
};

}
#endif

