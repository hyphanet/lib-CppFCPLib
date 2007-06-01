
#include "JobTicket.h"
#include "DefaultValues.h"
#include "zthread/Thread.h"
#include <boost/lexical_cast.hpp>

using namespace FCPLib;

JobTicket::JobTicket(std::string id_, Message::MessagePtr &cmd_)
  : id(id_),
    cmd(cmd_),
    async_(false),
    keep(false),
    waitTillSent_(false),
    timeout_(oneyear),
    isReprValid(false)
//    hasStream(false)
{
  lock.acquire();
  reqSentLock.acquire();
}

JobTicket&
JobTicket::async(bool async)
{
  isReprValid = false;
  async_ = async; return *this;
}

inline JobTicket&
JobTicket::keepJob(bool keep_)
{
  isReprValid = false;
  keep = keep_; return *this;
}
inline  JobTicket&
JobTicket::waitTillSent(bool wait_)
{
  isReprValid = false;
  waitTillSent_ = wait_; return *this;
}
inline  JobTicket&
JobTicket::timeout(int timeout)
{
  isReprValid = false;
  timeout_ = timeout; return *this;
}

const std::string&
JobTicket::getId() const
{
  return id;
}

const std::string&
JobTicket::commandName() const
{
  return cmd->getHeader();
}

const std::string&
JobTicket::getMessageText() const
{
  return cmd->toString();
}

void
JobTicket::wait(unsigned int timeout)
{
  if (!timeout){
    while (!lock.tryAcquire(100))
      ZThread::Thread::sleep(100);
    lock.release();
    return;
  }

  unsigned int then = (unsigned int) time(0);
  unsigned int elapsed;
  while (!reqSentLock.tryAcquire(100)){
    elapsed = (unsigned int) time(0) - then;
    if (elapsed < timeout){
                   Thread::sleep(1000)
      log().log(DEBUG, "wait:"+job->commandName()+":"+job->getId()+": job not dispatched, timeout in " +
        boost::lexical_cast<std::string>(timeout-elapsed));
      continue;
    }
  }
}

void
JobTicket::waitTillReqSent()
{
  reqSentLock.acquire();
}

void
JobTicket::putResult()
{
  lock.release();
}

const std::vector<Message::MessagePtr>&
JobTicket::getResponse() const
{
  return nodeResponse;
}

const std::string&
JobTicket::toString()
{
  if (isReprValid)
    return repr;

  repr = "";
  isReprValid = true;

  repr += "Job id=" + id + "\n";
  repr += getMessageText();
  repr += "async=" + boost::lexical_cast<std::string>(async_) + "\n";
  repr += "keepJob=" + boost::lexical_cast<std::string>(keep) + "\n";
  repr += "waitTillSent=" + boost::lexical_cast<std::string>(waitTillSent_) + "\n";
  repr += "timeout=" + boost::lexical_cast<std::string>(timeout_) + "\n";

  return repr;
}
