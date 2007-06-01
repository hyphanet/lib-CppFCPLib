

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
  Message::MessagePtr m;
  JobTicket::JobTicketPtr job;
  log().log(DETAIL, "FCPNode: manager thread starting");
  try {
    while (!Thread::interrupted()) {
      //check for incoming message from node
      log().log(NOISY, "_mgrThread: Testing for incoming message");
      if (s.dataAvailable()){
        log().log(DEBUG, "_mgrThread: Retrieving incoming message");
        m = Message::factory(s);
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
    // do some cleanup
  }
}

void
NodeThread::sendClientReq(JobTicket::JobTicketPtr &job)
{
  log().log(NOISY, "sendClientReq : top");
  if (job->commandName() != "WatchGlobal") {
    log().log(NOISY, "sendClientReq : about to add the job to the map");
    (*jobs)[job->getId()] = job;
    log().log(NOISY, "sendClientReq : added the job to the map");
  }

  s.send(job->getMessageText());
  job->timeQueued = (unsigned int) time(0);
  job->reqSentLock.release();
}

//
//void NodeThread::_hello(){
//    string message;
//    message = Message::toString("ClientHello", 2, "Name", this->name.c_str(), "ExpectedVersion", "2.0");
//    messageQSend->put(message);
//    logfile->log(DETAIL, message);
//
//    message = this->_rxMsg();
//    Message* nodeHello = new Message(message);
//    const char * tmp;
//    if ((tmp = nodeHello->getField("FCPVersion")) != NULL){
//        this->nodeFCPVersion = string(tmp);
//    }
//    if ((tmp = nodeHello->getField("Version")) != NULL){
//        this->nodeVersion = string(tmp);
//    }
//    if ((tmp = nodeHello->getField("Testnet")) != NULL){
//        this->nodeIsTestnet = (string(tmp) == "true") ? true : false;
//    }
//    if ((tmp = nodeHello->getField("CompressionCodes")) != NULL){
//        this->compressionCodes = atoi(tmp);
//    }
//
//    delete nodeHello;
//}


//
//string NodeThread::_rxMsg(){
//  string message = messageQReceive->get();
//  logfile->log(DETAIL, message);
//  return message;
//}
//
//void NodeThread::_on_rxMsg(std::string& message){
//  Message m(message);
//
//  logfile->log(DEBUG, m.toString());
//}

void
NodeThread::doMessage(Message::MessagePtr &message)
{
  JobTicket::JobTicketPtr job;
  if (message->getHeader() == "NodeHello"){
    job = jobs->find("__hello")->second;
    job->nodeResponse.push_back(message);
    job->putResult();
    return;
  }
}

