
#include "Log.h"
#include "ServerMessage.h"
#include "JobTicket.h"

#include <boost/lexical_cast.hpp>

using namespace FCPLib;

ServerMessage::Ptr
ServerMessage::factory(boost::shared_ptr<Server> s){
  ServerMessage::Ptr m;

  std::string header = s->readln();

  log().log(DETAIL, "NODE: " + header);

  if (header == "NodeHello"){
    m = Ptr( new NodeHelloMessage() );
  } else
  if (header == "CloseConnectionDuplicateName") {
    m = Ptr( new CloseConnectionDuplicateNameMessage() );
  } else
  if (header == "Peer") {
    m = Ptr( new PeerMessage() );
  } else
  if (header == "EndListPeers") {
    m = Ptr( new EndMessage() );
  } else
  if (header == "PeerNote") {
    m = Ptr( new PeerNoteMessage() );
  } else
  if (header == "EndListPeerNotes") {
    m = Ptr( new EndMessage() );
  } else
  if (header == "PeerRemoved") {
    m = Ptr( new PeerRemovedMessage() );
  } else
  if (header == "NodeData") {
    m = Ptr( new NodeDataMessage() );
  } else
  if (header == "ConfigData") {
    m = Ptr( new ConfigDataMessage() );
  } else
  if (header == "TestDDAReply") {
    m = Ptr( new TestDDAReplyMessage() );
  } else
  if (header == "TestDDAComplete") {
    m = Ptr( new TestDDACompleteMessage() );
  } else
  if (header == "SSKKeypair") {
    m = Ptr( new SSKKeypairMessage() );
  } else
  if (header == "PersistentGet") {
    m = Ptr( new PersistentGetMessage() );
  } else
  if (header == "PersistentPut") {
    m = Ptr( new PersistentPutMessage() );
  } else
  if (header == "PersistentPutDir") {
    m = Ptr( new PersistentPutDirMessage() );
  } else
  if (header == "URIGenerated") {
    m = Ptr( new URIGeneratedMessage() );
  } else
  if (header == "PutSuccessful") {
    m = Ptr( new PutSuccessfulMessage() );
  } else
  if (header == "PutFetchable") {
    m = Ptr( new PutFetchableMessage() );
  } else
  if (header == "DataFound") {
    m = Ptr( new DataFoundMessage() );
  } else
  if (header == "AllData") {
    m = Ptr( new AllDataMessage() );
  } else
  if (header == "StartedCompression") {
    m = Ptr( new StartedCompressionMessage() );
  } else
  if (header == "FinishedCompression") {
    m = Ptr( new FinishedCompressionMessage() );
  } else
  if (header == "SimpleProgress") {
    m = Ptr( new SimpleProgressMessage() );
  } else
  if (header == "EndListPersistentRequest") {
    m = Ptr( new EndMessage() );
  } else
  if (header == "PersistentRequestRemoved") {
    m = Ptr( new PersistentRequestRemovedMessage() );
  } else
  if (header == "PersistentRequestModified") {
    m = Ptr( new PersistentRequestModifiedMessage() );
  } else
  if (header == "PutFailed") {
    m = Ptr( new PutFailedMessage() );
  } else
  if (header == "GetFailed") {
    m = Ptr( new GetFailedMessage() );
  } else
  if (header == "ProtocolError") {
    m = Ptr( new ProtocolErrorMessage() );
  } else
  if (header == "IdentifierCollision") {
    m = Ptr( new IdentifierCollisionMessage() );
  } else
  if (header == "UnknownNodeIdentifier") {
    m = Ptr( new UnknownNodeIdentifierMessage() );
  } else
  if (header == "UnknownPeerNoteType") {
    m = Ptr( new UnknownPeerNoteTypeMessage() );
  } else
  if (header == "SubscribedUSKUpdate") {
    m = Ptr( new SubscribedUSKUpdate() );
  } else
    throw NotImplemented("Message " + header + " not implemented");

  m->message = Message::factory(header);

  m->read(s);

  return m;
}

void
ServerMessage::read(boost::shared_ptr<Server> s)
{
  std::string line;
  for (;;) {
    line = s->readln();

    log().log(DETAIL, "NODE: " + line);

    if ( line == "End" || line == "EndMessage" )
      break;

    int pos = line.find_first_of('=');

    message->setField(std::string(line.begin(), line.begin() + pos),
                      std::string(line.begin() + pos + 1, line.end()));
  }
}

const std::string&
ServerMessage::toString() const {
  return message->toString();
}

bool
IsLastPeer::operator()(const JobTicket::Ptr job) const
{
    std::string cmd( job->getCommandName() );
    if (cmd == "ListPeers")
      return false;
    else if (cmd == "ModifyPeer")
      return true;
    else if (cmd == "AddPeer")
      return true;
    else if (cmd == "ListPeer")
      return true;

    throw NotImplemented("Unknown command");
}

bool
IsLastPeerNote::operator()(const JobTicket::Ptr job) const
{
    std::string cmd( job->getCommandName() );
    if (cmd == "ListPeerNotes")
      return false;
    else if (cmd == "ModifyPeerNote")
      return true;

    throw NotImplemented("Unknown command");
}


bool
IsLastPutFailed::operator()(const JobTicketPtr job) const
{
  if ( boost::lexical_cast<int>( message->getField("Code") ) == 10 )
    return false;
  return true;
}

bool
IsLastGetFailed::operator()(const JobTicketPtr job) const
{
  if ( boost::lexical_cast<int>( message->getField("Code") ) == 25 )
    return false;
  return true;
}

bool
IsLastDataFound::operator()(const JobTicketPtr job) const
{
  GetJob::Ptr job_ = boost::dynamic_pointer_cast<GetJob, JobTicket>( job );

  if ( job_->getReturnType() != GetJob::Direct )
    return true;

  // non persistent job needs to ask for AllData
  return job_->isPersistent();
}

void
AllDataMessage::read(boost::shared_ptr<Server> s)
{
  server = s;
  std::string line;
  for (;;) {
    line = s->readln();

    log().log(DETAIL, "NODE: " + line);

    if ( line == "Data" )
      break;

    int pos = line.find_first_of('=');

    message->setField(std::string(line.begin(), line.begin() + pos),
                      std::string(line.begin() + pos + 1, line.end()));
  }

  bytesToRead = boost::lexical_cast<int>( message->getField("DataLength") );
  log().log(DETAIL, " ... " + message->getField("DataLength") + " bytes of data ...");
}

bool
AllDataMessage::isLast(const JobTicketPtr job) const
{
  log().log(NOISY, "AllDataMessage::isLast: top");
  GetJob::Ptr job_ = boost::dynamic_pointer_cast<GetJob, JobTicket>( job );

  std::ostream& stream = job_->getStream();
  char buf[1024];

  std::size_t bytes_available = bytesToRead;
  log().log(NOISY, "bytesToRead = " + boost::lexical_cast<std::string>( bytes_available ));
  while (bytes_available > 0) {
    std::size_t m = std::min<std::size_t>(bytes_available, 1024);
    server->read(buf, m);
    stream.write(buf, m);
    bytes_available -= m;
    log().log(DEBUG, "NODE: read "+ boost::lexical_cast<std::string>( m ) + " bytes of data, " + boost::lexical_cast<std::string>( bytes_available ) +  " still left");
  }
  stream.flush();

  return true;
}
