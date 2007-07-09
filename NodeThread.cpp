

#include "NodeThread.h"
#include "Log.h"
#include <ctime>

using namespace FCPLib;
using namespace ZThread;

NodeThread::NodeThread(std::string &host,
                       int port,
                       ZThread::CountedPtr<TQueue<JobTicket::JobTicketPtr> > &clientReqQueue_) throw()
  : clientReqQueue(clientReqQueue_),
    s(host, port),
    jobs( new std::map<std::string, JobTicket::JobTicketPtr>() )
{
}

void NodeThread::run(){
  ServerMessage::ServerMessagePtr m;
  JobTicket::JobTicketPtr job;
  log().log(DETAIL, "FCPNode: manager thread starting");
  try {
    while (!Thread::interrupted()) {
      //check for incoming message from node
      log().log(NOISY, "_mgrThread: Testing for incoming message");
      if (s.dataAvailable()){
        log().log(DEBUG, "_mgrThread: Retrieving incoming message");
        m = ServerMessage::factory(s);
        log().log(DEBUG, "_mgrThread: Got incoming message, dispatching");
        // dispatch the message
        doMessage(m);
      }
      //check for incoming message from client
      if (!clientReqQueue->empty()){
        log().log(DEBUG, "_mgrThread: Got incoming client req");
        job = clientReqQueue->get();
        log().log(DEBUG, "_mgrThread: Got incoming client req from the queue");
        log().log(DEBUG, job->toString());
        sendClientReq(job);
      }
    }
  } catch (ZThread::Synchronization_Exception& e) {
    log().log(DEBUG, "_mgrThread: Caught Synchronization_Exception");
  }
}

void
NodeThread::sendClientReq(JobTicket::JobTicketPtr &job)
{
  log().log(NOISY, "sendClientReq : top");
  if (job->getCommandName() != "WatchGlobal") {
    log().log(NOISY, "sendClientReq : about to add the job to the map");
    (*jobs)[job->getId()] = job;
    log().log(NOISY, "sendClientReq : added the job to the map");
  }

  s.send(job->getMessageText());
  job->timeQueued = (unsigned int) time(0);
  job->reqSentLock.release();
}

void
NodeThread::doMessage(ServerMessage::ServerMessagePtr &message)
{
  JobTicket::JobTicketPtr job;
  std::map<std::string, JobTicket::JobTicketPtr>::iterator it;

  it = jobs->find(message->getIdOfJob());
  if (it == jobs->end()) {
    log().log(DETAIL, "doMessage : received " + message->toString() + ", cannot find " + message->getIdOfJob() + " in started jobs");
    return;
  }

  job = it->second;
  job->nodeResponse.push_back(message);
  if (message->isLast(job->getCommandName())) {
    job->putResult();
  }
}

