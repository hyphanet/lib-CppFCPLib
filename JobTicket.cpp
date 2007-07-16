#include "JobTicket.h"
#include "DefaultValues.h"
#include "Log.h"

#include <boost/lexical_cast.hpp>


using namespace FCPLib;

JobTicket::Ptr
JobTicket::factory(std::string id, Message::Ptr cmd, bool keep)
{
   log().log(NOISY, "Creating " + cmd->getHeader());
   Ptr ret( new JobTicket() );

   ret->id = id;
   ret->cmd = cmd;

   ret->keep = keep;

   ret->lock.acquire();
   ret->reqSentLock.acquire();

   log().log(DEBUG, ret->toString());
   return ret;
}

const std::string&
JobTicket::getId() const {
  return id;
}

const std::string&
JobTicket::getCommandName() const
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
      ZThread::Thread::sleep(1000);
      log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job not dispatched, timeout in " +
        boost::lexical_cast<std::string>(timeout-elapsed));
      continue;
    }
    log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": timeout on send command");

    // TODO: should I maybe create a result here, and then when retrieving
    // result throw an exception??
    throw std::runtime_error("command timeout");
  }
  log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job now dispatched");
  while (!lock.tryAcquire(100)){
    elapsed = (unsigned int) time(0) - then;
    if (elapsed < timeout) {
      ZThread::Thread::sleep(2000);
      log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": waiting for node response, timeout in " +
        boost::lexical_cast<std::string>(timeout-elapsed));
      continue;
    }
    log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": timeout on node response");

    throw std::runtime_error("command timeout");
  }
  log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job complete");
  lock.release();
}

void
JobTicket::waitTillReqSent()
{
  reqSentLock.acquire();
}

const std::string&
JobTicket::toString()
{
  if (isReprValid)
    return repr;

  repr = "";
  isReprValid = true;

  repr += "Job id=" + id + " " +
             " keepJob=" + boost::lexical_cast<std::string>(keep) + "\n";
  repr += "Message=" + cmd->getHeader();

  // TODO:
  // add representation of hasResult and Result

  return repr;
}


void
JobTicket::putResult()
{
  ZThread::Guard<ZThread::Mutex> g(access);
  _isFinished = true;
  lock.release();
}
