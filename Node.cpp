
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

  // TODO: put information received from NODE somewhere
}

Node::~Node()
{
  executor.interrupt();
}

FCPMultiMessageResponse::FCPMultiMessageResponsePtr
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

  return boost::dynamic_pointer_cast<FCPMultiMessageResponse, FCPResult>(job->getResult());
}

FCPMultiMessageResponse::FCPMultiMessageResponsePtr
Node::listPeerNotes(const std::string& identifier)
{
  Message::MessagePtr m = Message::factory( std::string("ListPeerNotes") );
  m->setField("NodeIdentifier", identifier);

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the EndListPeerNotes");
  job->wait(0);
  log().log(DEBUG, "EndListPeers arrived");

  return boost::dynamic_pointer_cast<FCPMultiMessageResponse, FCPResult>(job->getResult());
}

void
Node::addPeer(const std::string &value, bool isURL = false) {
  Message::MessagePtr m = Message::factory( std::string("AddPeer") );
  if (!isURL)
    m->setField("File", value);
  else
    m->setField("URL", value);

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for AddPeer to be sent");
  job->waitTillReqSent();
  log().log(DEBUG, "AddPeer to was sent");
}


void
Node::addPeer(const std::map<std::string, std::string> &message)
{
  Message::MessagePtr m = Message::factory( std::string("AddPeer") );

  m->setFields(message);

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for AddPeer to be sent");
  job->waitTillReqSent();
  log().log(DEBUG, "AddPeer to was sent");
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::modifyPeer(const std::string & nodeIdentifier,
                 bool allowLocalAddresses = false,
                 bool isDisabled = false,
                 bool isListenOnly = false)
{
  Message::MessagePtr m = Message::factory( std::string("ModifyPeer") );

  m->setField("NodeIdentifier", nodeIdentifier);
  if (allowLocalAddresses)
    m->setField("AllowLocaAddresses", "true");
  if (isDisabled)
    m->setField("IsDisabled", "true");
  if (isListenOnly)
    m->setField("IsListenOnly", "true");


  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the Peer");
  job->wait(0);
  log().log(DEBUG, "Peer received");

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::modifyPeerNote(const std::string & nodeIdentifier,
                     const std::string & noteText,
                     int peerNoteType = 1)
{
  Message::MessagePtr m = Message::factory( std::string("ModifyPeerNote") );

  m->setField("NodeIdentifier", nodeIdentifier);
  m->setField("NoteText", noteText);
  m->setField("PeerNoteType", "1");

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the PeerNote");
  job->wait(0);
  log().log(DEBUG, "PeerNote received");

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::removePeer(const std::string &identifier)
{
  Message::MessagePtr m = Message::factory( std::string("RemovePeer") );

  m->setField("NodeIdentifier", identifier);


  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the PeerRemoved");
  job->wait(0);
  log().log(DEBUG, "PeerRemoved received");

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::getNode(bool withPrivate = false,
              bool withVolatile = false)
{
  Message::MessagePtr m = Message::factory( std::string("GetNode") );

  if (withPrivate)
    m->setField("WithPrivate", "True");
  if (withVolatile)
    m->setField("WithVolatile", "True");

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the NodeData");
  job->wait(0);
  log().log(DEBUG, "NodeData received");

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}
