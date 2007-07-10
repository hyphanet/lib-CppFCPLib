
#include "Node.h"
#include "Log.h"
#include <boost/lexical_cast.hpp>
#include <typeinfo>
#include "Exceptions.h"
#include "FCPErrorResponse.h"

using namespace FCPLib;

std::string
Node::_getUniqueId() {
    char newid[100];
    sprintf(newid, "id%d", (int) time(0));
    return std::string(newid);
}

const Message::MessagePtr
Node::getNodeHelloMessage() const
{
  return nodeHelloMessage;
}

void
Node::checkProtocolError(JobTicket::JobTicketPtr &job)
{
  if (typeid(job->getResult()).name() == "FCPErrorResponse") {
    Message::MessagePtr lastMessage =
       ( boost::dynamic_pointer_cast<FCPErrorResponse, FCPResult> (job->getResult()) )
          ->getMessages().back()->getMessage();
    throw new FCPException(lastMessage);
  }
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

  // check if CloceConnectionDuplicateName or ProtocolError has arrived
  checkProtocolError(job); // throws

  nodeHelloMessage = ( boost::dynamic_pointer_cast<FCPErrorResponse, FCPResult> (job->getResult()) )
                          ->getMessages().back()->getMessage();
}

Node::~Node()
{
  executor.interrupt();
}

FCPMultiMessageResponse::FCPMultiMessageResponsePtr
Node::listPeers(const AdditionalFields& fields)
{
  Message::MessagePtr m = Message::factory( std::string("ListPeers") );
  if (fields.hasField("WithMetadata")) m->setField("WithMetadata", fields.getField("WithMetadata"));
  if (fields.hasField("WithVolatile")) m->setField("WithVolatile", fields.getField("WithVolatile"));

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the EndListPeers");
  job->wait(0);
  log().log(DEBUG, "EndListPeers arrived");

  // NOTE: error should never happen hear...
  checkProtocolError(job); // throws

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

  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(job); // throws

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

  // should I check for the errors? how do I know that the operation was successful
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

  // should I check for the errors? how do I know that the operation was successful
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::modifyPeer(const std::string & nodeIdentifier,
                 const AdditionalFields& fields)
{
  Message::MessagePtr m = Message::factory( std::string("ModifyPeer") );

  m->setField("NodeIdentifier", nodeIdentifier);
  if (fields.hasField("AllowLocalAddresses")) m->setField("AllowLocalAddresses", fields.getField("AllowLocalAddresses"));
  if (fields.hasField("IsDisabled")) m->setField("IsDisabled", fields.getField("IsDisabled"));
  if (fields.hasField("IsListenOnly")) m->setField("IsListenOnly", fields.getField("IsListenOnly"));

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
  m->setField("PeerNoteType", "1");  // TODO: change to peerNoteType once it is used

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the PeerNote");
  job->wait(0);
  log().log(DEBUG, "PeerNote received");

  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(job); // throws

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

  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(job); // throws

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::getNode(const AdditionalFields& fields)
{
  Message::MessagePtr m = Message::factory( std::string("GetNode") );

  if (fields.hasField("WithPrivate")) m->setField("WithPrivate", fields.getField("WithPrivate"));
  if (fields.hasField("WithVolatile")) m->setField("WithVolatile", fields.getField("WithVolatile"));

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the NodeData");
  job->wait(0);
  log().log(DEBUG, "NodeData received");

  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(job); // throws

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::getConfig(const AdditionalFields& fields)
{
  Message::MessagePtr m = Message::factory( std::string("GetConfig") );

  if (fields.hasField("WithCurrent")) m->setField("WithCurrent", fields.getField("WithCurrent"));
  if (fields.hasField("WithDefault")) m->setField("WithDefault", fields.getField("WithDefault"));
  if (fields.hasField("WithSortOrder")) m->setField("WithSortOrder", fields.getField("WithSortOrder"));
  if (fields.hasField("WithExpertFlag")) m->setField("WithExpertFlag", fields.getField("WithExpertFlag"));
  if (fields.hasField("WithForceWriteFlag")) m->setField("WithForceWriteFlag", fields.getField("WithForceWriteFlag"));
  if (fields.hasField("WithShortDescription")) m->setField("WithShortDescription", fields.getField("WithShortDescription"));
  if (fields.hasField("WithLongDescription")) m->setField("WithLongDescription", fields.getField("WithLongDescription"));

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the ConfigData");
  job->wait(0);
  log().log(DEBUG, "ConfigData received");

  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(job); // throws

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::modifyConfig(Message::MessagePtr m)
{
  if (m->getHeader() != "ModifyConfig")
    throw new std::logic_error("ModifyConfig message expected, " + m->getHeader() + " received");

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the ConfigData");
  job->wait(0);
  log().log(DEBUG, "ConfigData received");

  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(job); // throws

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPTestDDAReplyResponse::FCPTestDDAReplyResponsePtr
Node::testDDARequest(std::string dir, bool read, bool write)
{
  Message::MessagePtr m = Message::factory( std::string("TestDDARequest") );

  m->setField("Directory", dir);
  m->setField("WantReadDirectory", boost::lexical_cast<std::string>(read));
  m->setField("WantWriteDirectory", boost::lexical_cast<std::string>(write));

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );

  log().log(DEBUG, "waiting for the TestDDAReply");
  job->wait(0);
  log().log(DEBUG, "TestDDAReply received");

  // check if protocol error has occured
  checkProtocolError(job); // throws

  return boost::dynamic_pointer_cast<FCPTestDDAReplyResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::testDDAResponse(std::string dir, std::string readContent)
{
  Message::MessagePtr m = Message::factory( std::string("TestDDAResponse") );

  m->setField("Directory", dir);
  if (readContent != "")
    m->setField("ReadContent", readContent);

  JobTicket::JobTicketPtr job = JobTicket::factory( "__global", m, false, false, false, 0 );

  log().log(DEBUG, "waiting for the TestDDA");
  job->wait(0);
  log().log(DEBUG, "TestDDAResponse received");

  // check if protocol error has occured
  checkProtocolError(job); // throws

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPOneMessageResponse::FCPOneMessageResponsePtr
Node::generateSSK(std::string identifier)
{
  Message::MessagePtr m = Message::factory( std::string("GenerateSSK") );
  m->setField("Identifier", identifier);

  JobTicket::JobTicketPtr job = JobTicket::factory( identifier, m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for SSKKeypair");
  job->wait(0);
  log().log(DEBUG, "SSKKeypair arrived");

  checkProtocolError(job); // throws

  return boost::dynamic_pointer_cast<FCPOneMessageResponse, FCPResult>(job->getResult());
}

FCPMultiMessageResponse::FCPMultiMessageResponsePtr
Node::putData(const std::string URI, const std::string data, const std::string id, const AdditionalFields& fields )
{
  Message::MessagePtr m = Message::factory( std::string("ClientPut"), true );

  m->setField("URI", URI);
  m->setField("Identifier", id == "" ? _getUniqueId() : id);
  if (fields.hasField("Metadata.ContentType")) m->setField("Metadata.ContentType", fields.getField("Metadata.ContentType"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  if (fields.hasField("GetCHKOnly")) m->setField("GetCHKOnly", fields.getField("GetCHKOnly"));
  if (fields.hasField("Global")) m->setField("Global", fields.getField("Global"));
  if (fields.hasField("DontCompress")) m->setField("DontCompress", fields.getField("DontCompress"));
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence")) m->setField("Persistence", fields.getField("Persistence"));
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  m->setField("UploadFrom", "direct");
  m->setField("Data", data);

  JobTicket::JobTicketPtr job = JobTicket::factory( m->getField("Identifier"), m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the put successful");
  job->wait(0);
  log().log(DEBUG, "put successful arrived");

  return boost::dynamic_pointer_cast<FCPMultiMessageResponse, FCPResult>(job->getResult());
}

FCPMultiMessageResponse::FCPMultiMessageResponsePtr
Node::putRedirect(const std::string URI, const std::string target, const std::string id, const AdditionalFields& fields )
{
  Message::MessagePtr m = Message::factory( std::string("ClientPut"));

  m->setField("URI", URI);
  m->setField("Identifier", id == "" ? _getUniqueId() : id);
  if (fields.hasField("mimetype")) m->setField("Metadata.ContentType", fields.getField("mimetype"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  if (fields.hasField("Global")) m->setField("Global", fields.getField("Global"));
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence")) m->setField("Persistence", fields.getField("Persistence"));
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  m->setField("TargetURI", target);

  JobTicket::JobTicketPtr job = JobTicket::factory( m->getField("Identifier"), m, false, false, false, 0 );
  log().log(DEBUG, job->toString());
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for the put successful");
  job->wait(0);
  log().log(DEBUG, "put successful arrived");

  return boost::dynamic_pointer_cast<FCPMultiMessageResponse, FCPResult>(job->getResult());
}
