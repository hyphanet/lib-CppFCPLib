#include "JobTicket.h"
#include "DefaultValues.h"
#include "Log.h"

#include <boost/lexical_cast.hpp>
#include "Utils.h"
#include "Node.h"


using namespace FCPLib;

JobTicket::Ptr
JobTicket::factory(Node* n, std::string id, Message::Ptr cmd)
{
   log().log(NOISY, "Creating " + cmd->getHeader());
   Ptr ret( new JobTicket() );
   ret->init(n, id, cmd);

   return ret;
}

void
JobTicket::init(Node* n, std::string &id, Message::Ptr cmd)
{
   this->node = n;
   this->id = id;
   this->cmd = cmd;

   this->lock.acquire();
   this->reqSentLock.acquire();

   log().log(DEBUG, this->toString());
}

void
JobTicket::wait(unsigned int timeout)
{
  if (!timeout){
    while (!lock.tryAcquire(100)) {
      if (! node->isAlive() ) {
        if ( node->hasFailure() ) throw node->getFailure();
        else throw FCPNodeFailure( "node thread is not alive" );
      }
      ZThread::Thread::sleep(100);
    }
    lock.release();
    return;
  }

  unsigned int then = (unsigned int) time(0);
  unsigned int elapsed;
  while (!reqSentLock.tryAcquire(100)){
    if (! node->isAlive() ) {
      if ( node->hasFailure() ) throw node->getFailure();
      else throw FCPNodeFailure( "node thread is not alive" );
    }
    elapsed = (unsigned int) time(0) - then;
    if (elapsed < timeout){
      ZThread::Thread::sleep(1000);
      log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job not dispatched, timeout in " +
        boost::lexical_cast<std::string>(timeout-elapsed));
      continue;
    }
    log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": timeout on send command");

    throw CommandTimeout("Timeout sending " + this->getCommandName());
  }
  log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job now dispatched");
  while (!lock.tryAcquire(100)){
    if (! node->isAlive() ) {
      if ( node->hasFailure() ) throw node->getFailure();
      else throw FCPNodeFailure( "node thread is not alive" );
    }
    elapsed = (unsigned int) time(0) - then;
    if (elapsed < timeout) {
      ZThread::Thread::sleep(2000);
      log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": waiting for node response, timeout in " +
        boost::lexical_cast<std::string>(timeout-elapsed));
      continue;
    }
    log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": timeout on node response");

    throw CommandTimeout("Timeout sending " + this->getCommandName());
  }
  log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job complete");
  lock.release();
}

void
JobTicket::waitTillReqSent(unsigned int timeout)
{
  unsigned int then = (unsigned int) time(0);
  unsigned int elapsed;
  while (!reqSentLock.tryAcquire(100)){
    if (! node->isAlive() ) {
      if ( node->hasFailure() ) throw node->getFailure();
      else throw FCPNodeFailure( "node thread is not alive" );
    }
    elapsed = (unsigned int) time(0) - then;
    if (elapsed < timeout){
      ZThread::Thread::sleep(1000);
      log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": job not dispatched, timeout in " +
        boost::lexical_cast<std::string>(timeout-elapsed));
      continue;
    }
    log().log(DEBUG, "wait:"+this->getCommandName()+":"+this->getId()+": timeout on send command");
    throw CommandTimeout("Timeout sending " + this->getCommandName());
  }
}

const std::string&
JobTicket::toString()
{
  if (isReprValid)
    return repr;

  repr = "";
  isReprValid = true;

  repr += "Job id=" + id +
             " keepJob=" + Converter::toString( keep ) +
             " global=" + Converter::toString( global ) +
             " persistent=" + Converter::toString( persistent ) + "\n";

  repr += "Message=" + cmd->getHeader();

  return repr;
}

void
JobTicket::putResult()
{
  ZThread::Guard<ZThread::Mutex> g(access);
  _isFinished = true;
  lock.release();
}

GetJob::Ptr
GetJob::factory(Node *n, std::string id, Message::Ptr cmd)
{
  log().log(NOISY, "Creating " + cmd->getHeader());
  Ptr ret( new GetJob() );
  ret->init(n, id, cmd);

  return ret;
}

const std::string&
GetJob::toString()
{
  if (isReprValid)
    return repr;

  repr = "";
  isReprValid = true;

  repr += "Job id=" + id + " " +
             " keepJob=" + Converter::toString( keep ) +
             " global=" + Converter::toString( global ) +
             " persistent=" + Converter::toString( persistent ) +
             " returnType=" + boost::lexical_cast<std::string>( retType )+ "\n";

  repr += "Message=" + cmd->getHeader();

  return repr;
}
