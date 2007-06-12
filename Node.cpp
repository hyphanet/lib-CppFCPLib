
#include "Node.h"
#include "Log.h"

using namespace FCPLib;

std::string
Node::_getUniqueId() {
    char newid[100];
    sprintf(newid, "id%d", (int) time(0));
    return string(newid);
}

Node::Node(std::string name_, std::string host, int port)
  : name(name_),
    clientReqQueue( new TQueue<JobTicket::JobTicketPtr>() )
{
  if (!name.size())
    name = Node::_getUniqueId();
  log().log(DEBUG, "Node started name=" + name + "\n");

  nodeThread = new NodeThread(host, port, clientReqQueue);
  executor.execute( nodeThread );

  Message::MessagePtr m = Message::factory(std::string("ClientHello"));
  m->setField("Name", name);
  m->setField("ExpectedVersion", "2.0");

  log().log(DEBUG, "Creating ClientHello\n");
  JobTicket::JobTicketPtr job = JobTicket::factory("__hello", m, false, false, false, 0);
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the NodeHello");
  job->wait(0);
  log().log(DEBUG, "NodeHello arrived");
}

Node::~Node()
{
  executor.interrupt();
}

FCPListPeersResult::FCPListPeersResultPtr
Node::listPeers(bool withMetaData = false,
                bool withVolatile = false)
{
  Message::MessagePtr m = Message::factory( std::string("ListPeers") );
  m->setField("WithMetadata", withMetaData ? "true" : "false");
  m->setField("WithVolatila", withVolatile ? "true" : "false");

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the EndListPeers");
  job->wait(0);
  log().log(DEBUG, "EndListPeers arrived");

  return boost::dynamic_pointer_cast<FCPListPeersResult, FCPResult>(job->getResult());
}

FCPListPeerNotesResult::FCPListPeerNotesResultPtr
Node::listPeerNotes(std::string& identifier)
{
  Message::MessagePtr m = Message::factory( std::string("ListPeerNotes") );
  m->setField("NodeIdentifier", identifier);

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the EndListPeerNotes");
  job->wait(0);
  log().log(DEBUG, "EndListPeers arrived");

  return boost::dynamic_pointer_cast<FCPListPeerNotesResult, FCPResult>(job->getResult());
}
