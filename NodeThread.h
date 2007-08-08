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
typedef ZThread::CountedPtr< JobTicketQueue > JobTicketQueuePtr;

class NodeThread : public ZThread::Runnable {
  friend class Node;

  Node* node;

  JobTicketQueuePtr clientReqQueue;
  std::string host_;
  int port_;
  boost::shared_ptr<Server> s;

  std::map<std::string, JobTicket::Ptr > jobs[2]; // 0 -- local jobs, 1 -- global jobs

  NodeThread(Node* n, std::string &host, int port, JobTicketQueuePtr clientReqQueue_) throw();

  void sendClientReq(JobTicket::Ptr job);
  void doMessage(ServerMessage::Ptr message);
public:
  void run();
  ~NodeThread() {
    jobs[0].clear();
    jobs[1].clear();
  }
};

}
#endif

