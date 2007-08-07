

#include <ctime>
#include <boost/lexical_cast.hpp>

#include "NodeThread.h"
#include "Log.h"
#include "Node.h"

using namespace FCPLib;
using namespace ZThread;

NodeThread::NodeThread(Node* n,
                       std::string &host,
                       int port,
                       JobTicketQueuePtr clientReqQueue_) throw()
  : node(n),
    clientReqQueue(clientReqQueue_),
    host_(host),
    port_(port),
    s(new Server( host_, port_ ))
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
    log().log(ERROR, e.what());
    node->setIsAlive(false);
  } catch (std::runtime_error& e) {
    // some error has occured, keep the thread so you can access the isAlive and getFailure
    log().log(ERROR, "_mgrThreag: Caught std::runtime_error");
    log().log(ERROR, e.what());
    node->setIsAlive( false );
    node->setFailure( e.what() );
  } catch (std::exception& e) {
    // some error has occured, keep the thread so you can access the isAlive and getFailure
    log().log(ERROR, "_mgrThreag: Caught std::exception");
    log().log(ERROR, e.what());
    node->setIsAlive( false );
    node->setFailure( e.what() );
  } catch (...) {
    // thread is stopped and we don't know what has happend
    log().log(ERROR, "_mgrThreag: Caught something else");
    node->setIsAlive(false);
    node->setFailure( "unknown error" );
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
  int isGlobal = tmp == "false" ? 0 : 1;

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
      JobTicket::Ptr job = JobTicket::factory( this->node, m->getField("Identifier"), m );
      job->setGlobal(isGlobal).setPersistent(true);
      jobs[isGlobal][m->getField("Identifier")] = job;
      return;
    }
  }

  job = it->second;

  if ( message->isLast( job ) ) {
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

