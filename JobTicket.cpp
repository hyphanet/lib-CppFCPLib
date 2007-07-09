#include "JobTicket.h"
#include "DefaultValues.h"
#include "Log.h"

#include <boost/lexical_cast.hpp>

using namespace FCPLib;

JobTicket::JobTicketPtr
JobTicket::factory(std::string id, Message::MessagePtr cmd,
                   bool async, bool keep, bool waitTillSent,
                   int timeout)
  {
     JobTicketPtr ret( new JobTicket() );

     ret->id = id;
     ret->cmd = cmd;

     ret->async = async;
     ret->keep = keep;
     ret->waitTillSent = waitTillSent;
     ret->timeout = timeout;


     ret->lock.acquire();
     ret->reqSentLock.acquire();

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
    throw new std::runtime_error("command timeout");
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

    // TODO: should I maybe create a result here, and then when retrieving
    // result throw an exception??
    throw new std::runtime_error("command timeout");
  }
  log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job complete");
  lock.release();
}

void
JobTicket::waitTillReqSent()
{
  reqSentLock.acquire();
}

const std::vector<ServerMessage::ServerMessagePtr>&
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
  repr += "async=" + boost::lexical_cast<std::string>(async) + "\n";
  repr += "keepJob=" + boost::lexical_cast<std::string>(keep) + "\n";
  repr += "waitTillSent=" + boost::lexical_cast<std::string>(waitTillSent) + "\n";
  repr += "timeout=" + boost::lexical_cast<std::string>(timeout) + "\n";

  // TODO:
  // add representation of hasResult and Result

  return repr;
}

bool
JobTicket::hasResult() const {
  return _hasResult;
}

const FCPResult::FCPResultPtr
JobTicket::getResult() const {
  if (!_hasResult)
    throw new std::runtime_error("Result is not ready.");
  return result;
}

void
JobTicket::putResult() {
  result = FCPResult::factory(cmd->getHeader(), nodeResponse);
  _hasResult = true;
  lock.release();
}
