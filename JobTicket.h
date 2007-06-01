#ifndef JOBTICKET_H__
#define JOBTICKET_H__

#include "Log.h"
#include "Message.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "zthread/FastMutex.h"

namespace FCPLib {

class NodeThread;

class JobTicket {
  std::string id;
  Message::MessagePtr cmd;
  std::vector<Message::MessagePtr> nodeResponse;
  bool async_;
  bool keep;
  bool waitTillSent_;
  int timeout_;

  std::string repr;
  bool isReprValid;

//  bool hasStream;
//  ostream stream;

  ZThread::FastMutex lock;
  ZThread::FastMutex reqSentLock;
  int timeQueued;

  void putResult();
public:
  typedef boost::shared_ptr<JobTicket> JobTicketPtr;

  JobTicket(std::string id_, Message::MessagePtr &cmd_);
  inline JobTicket& async(bool async_);
  inline JobTicket& keepJob(bool keep_);
  inline JobTicket& waitTillSent(bool wait);
  inline JobTicket& timeout(int timeout);

  const std::string& commandName() const;
  const std::string& getId() const;
  const std::string& getMessageText() const;

  void wait(unsigned int timeout_=0);
  void waitTillReqSent();

  const std::vector<Message::MessagePtr>& getResponse() const;
  const std::string& toString();


  friend class NodeThread;
};

}

#endif
