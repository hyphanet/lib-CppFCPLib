

#include <ctime>
#include <boost/lexical_cast.hpp>

#include "NodeThread.h"
#include "Log.h"


using namespace FCPLib;
using namespace ZThread;

NodeThread::NodeThread(std::string &host,
                       int port,
                       JobTicketQueuePtr clientReqQueue_) throw()
  : clientReqQueue(clientReqQueue_),
    host_(host),
    port_(port),
    s(new Server( host_, port_ )),
    isAlive_(true)
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
    // thread was interupted, normal way to shutdown the thread
    // this object will be destroyed
    log().log(ERROR, "_mgrThread: Caught Synchronization_Exception");
    return;
  } catch (std::runtime_error& e) {
    // some error has occured, keep the thread so you can access the isAlive and getFailure
    log().log(ERROR, "_mgrThreag: Caught std::runtime_error");
    isAlive_ = false;
    exception = ZThread::CountedPtr<std::exception> ( new std::runtime_error(e) );
  } catch (std::exception& e) {
    // some error has occured, keep the thread so you can access the isAlive and getFailure
    log().log(ERROR, "_mgrThreag: Caught std::exception");
    isAlive_ = false;
    exception = ZThread::CountedPtr<std::exception> ( new std::exception(e) );
  } catch (...) {
    // thread is stopped and
    log().log(ERROR, "_mgrThreag: Caught something else");
    isAlive_ = false;
    return;
  }
  try {
    while (!Thread::interrupted()) {
      // dummy loop, wait untill interrupt
      Thread::sleep(1000);
      Thread::yield();
    }
  } catch (ZThread::Synchronization_Exception& e) {
    // thread was interupted, normal way to shutdown the thread
    // this object will be destroyed
    log().log(ERROR, "_mgrThread: Caught Synchronization_Exception");
    return;
  }
}

void
NodeThread::sendClientReq(JobTicket::Ptr job)
{
  log().log(NOISY, "sendClientReq : top");
  if (job->getCommandName() != "WatchGlobal") {
    log().log(NOISY, "sendClientReq : about to add the job to the map");
    jobs[job->isGlobal() ? 1 : 0][job->getId()] = job;
    log().log(NOISY, "sendClientReq : added the job to the map");
  }

  s->send(job->getCommand());
  job->timeQueued = (unsigned int) time(0);
}

void
NodeThread::doMessage(ServerMessage::Ptr message)
{
  JobTicket::Ptr job;
  std::map<std::string, JobTicket::Ptr>::iterator it;

  std::string tmp = message->getMessage()->getField("Global");
  tmp = tmp == "" ? "false" : tmp;
  int isGlobal = boost::lexical_cast<int>(tmp);

  it = jobs[isGlobal].find(message->getIdOfJob());
  if (it == jobs[isGlobal].end()) {
    log().log(DETAIL, "doMessage : received " + message->getMessage()->getHeader() + ", cannot find " + message->getIdOfJob() + " in started jobs");
    /// message from global queue or error
    Message::Ptr m = message->getMessage();
    if (!isGlobal) { // error
      log().log(DEBUG, "doMessage : received error message");
      // TODO: create a mean of passing error messages to client programme
      return;
    } else { // global queue, create a job
      log().log(DEBUG, "doMessage : received message from a global queue");
      if ( m->getField("Identifier") == "" ) {
        // should never happen
        log().log(ERROR, "doMessage : global message does not contain identifier !???");
        return;
      }
      JobTicket::Ptr job = JobTicket::factory( m->getField("Identifier"), m );
      jobs[1][m->getField("Identifier")] = job;
      return;
    }
  }

  job = it->second;

  if ( message->isLast( job->getCommandName() ) ) {
    log().log(NOISY, "doMessage : last message for the job");
    job->putResponse(1, message);
    job->putResult();

    if (!job->keep) {
      log().log(NOISY, "doMessage : job should not be kept, erasing");
      jobs[isGlobal].erase( it );
    }
  }
  else {
    job->putResponse(0, message);
  }
}

