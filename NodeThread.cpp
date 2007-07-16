

#include "NodeThread.h"
#include "Log.h"
#include <ctime>

using namespace FCPLib;
using namespace ZThread;

NodeThread::NodeThread(std::string &host,
                       int port,
                       ZThread::CountedPtr<TQueue<JobTicket::Ptr> > clientReqQueue_) throw()
  : clientReqQueue(clientReqQueue_),
    host_(host),
    port_(port),
    s(new Server( host_, port_ )),
    isAlive_(true),
    jobs( new std::map<std::string, JobTicket::Ptr>() )
{
}

void NodeThread::run(){
  ServerMessage::Ptr m;
  JobTicket::Ptr job;
  log().log(DETAIL, "FCPNode: manager thread starting");
  try {
    while (!Thread::interrupted()) {
      //check for incoming message from node
      log().log(NOISY, "_mgrThread: Testing for incoming message");
      if (s->dataAvailable()){
        log().log(DEBUG, "_mgrThread: Retrieving incoming message");
        m = ServerMessage::factory(s);
        log().log(DEBUG, "_mgrThread: Got incoming message, dispatching");
        // dispatch the message
        doMessage(m);
      }
      //check for incoming message from client
      log().log(NOISY, "_mgrThread: Testing for incoming req");
      if (!clientReqQueue->empty()){
        log().log(DEBUG, "_mgrThread: Got incoming client req");
        job = clientReqQueue->get();
        log().log(DEBUG, "_mgrThread: Got incoming client req from the queue");
        log().log(NOISY, job->toString());
        sendClientReq(job);
      }
      Thread::sleep(100);  // do I need this?
    }
  } catch (ZThread::Synchronization_Exception& e) {
    log().log(ERROR, "_mgrThread: Caught Synchronization_Exception");
    isAlive_ = false;
    exception = ZThread::CountedPtr<std::exception> ( new std::runtime_error(e.what()) );
    return;
  } catch (std::runtime_error& e) {
    log().log(ERROR, "_mgrThreag: Caught std::runtime_error");
    isAlive_ = false;
    exception = ZThread::CountedPtr<std::exception> ( new std::runtime_error(e) );
    return;
  } catch (std::exception& e) {
    log().log(ERROR, "_mgrThreag: Caught std::exception");
    isAlive_ = false;
    exception = ZThread::CountedPtr<std::exception> ( new std::exception(e) );
    return;
  } catch (...) {
    log().log(ERROR, "_mgrThreag: Caught something else");
    isAlive_ = false;
    return;
  }
  // TODO: catch more specific exceptions as well
  // TODO: what does happen to this object when exception is thrown?
}

void
NodeThread::sendClientReq(JobTicket::Ptr job)
{
  log().log(NOISY, "sendClientReq : top");
  if (job->getCommandName() != "WatchGlobal") {
    log().log(NOISY, "sendClientReq : about to add the job to the map");
    (*jobs)[job->getId()] = job;
    log().log(NOISY, "sendClientReq : added the job to the map");
  }

  s->send(job->getMessageText());
  job->timeQueued = (unsigned int) time(0);
  job->reqSentLock.release();
}

void
NodeThread::doMessage(ServerMessage::Ptr message)
{
  JobTicket::Ptr job;
  std::map<std::string, JobTicket::Ptr>::iterator it;

  it = jobs->find(message->getIdOfJob());
  if (it == jobs->end()) {
    log().log(DETAIL, "doMessage : received " + message->toString() + ", cannot find " + message->getIdOfJob() + " in started jobs");
    return;
  }

  job = it->second;

  if ( message->isLast( job->getCommandName() ) ) {
    job->putResponse(1, message);
    job->putResult();
  }
  else
    job->putResponse(0, message);
}

