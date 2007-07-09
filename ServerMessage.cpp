
#include "Log.h"
#include "ServerMessage.h"
#include "PeerNoteMessage.h"
#include "EndMessage.h"
#include "PeerMessage.h"
#include "NodeHelloMessage.h"
#include "PeerRemovedMessage.h"
#include "NodeDataMessage.h"
#include "ConfigDataMessage.h"
#include "SimpleProgressMessage.h"
#include "StartedCompressionMessage.h"
#include "FinishedCompressionMessage.h"
#include "PutSuccessfulMessage.h"
#include "URIGeneratedMessage.h"
#include "PutFailedMessage.h"
#include "GetFailedMessage.h"
#include "ProtocolErrorMessage.h"
#include "IdentifierCollisionMessage.h"
#include "UnknownNodeIdentifierMessage.h"
#include "UnknownPeerNoteTypeMessage.h"
#include "PersistentRequestRemovedMessage.h"

using namespace FCPLib;


ServerMessage::ServerMessagePtr
ServerMessage::factory(Server &s){
  ServerMessage::ServerMessagePtr m;
  static char line[1000];

  s.readln(line, 1000);
  line[strlen(line)-1] = 0;
  std::string header = std::string(line);

  log().log(DETAIL, "NODE: " + header);

  if (header == "NodeHello"){
    m = ServerMessagePtr( new NodeHelloMessage() );
  } else
  if (header == "Peer") {
    m = ServerMessagePtr( new PeerMessage() );
  } else
  if (header == "EndListPeers") {
    m = ServerMessagePtr( new EndMessage() );
  } else
  if (header == "PeerNote") {
    m = ServerMessagePtr( new PeerNoteMessage() );
  } else
  if (header == "EndListPeerNotes") {
    m = ServerMessagePtr( new EndMessage() );
  } else
  if (header == "PeerRemoved") {
    m = ServerMessagePtr( new PeerRemovedMessage() );
  } else
  if (header == "NodeData") {
    m = ServerMessagePtr( new NodeDataMessage() );
  } else
  if (header == "ConfigData") {
    m = ServerMessagePtr( new ConfigDataMessage() );
  } else
  if (header == "TestDDAReply") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "TestDDAComplete") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "SSKKeypair") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "PersistentGet") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "PersistentPut") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "PersistentPutDir") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "URIGenerated") {
    m = ServerMessagePtr( new URIGeneratedMessage() );
  } else
  if (header == "PutSuccessful") {
    m = ServerMessagePtr( new PutSuccessfulMessage() );
  } else
  if (header == "PutFetchable") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "DataFound") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "AllData") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "StartedCompression") {
    m = ServerMessagePtr( new StartedCompressionMessage() );
  } else
  if (header == "FinishedCompression") {
    m = ServerMessagePtr( new FinishedCompressionMessage() );
  } else
  if (header == "SimpleProgress") {
    m = ServerMessagePtr( new SimpleProgressMessage() );
  } else
  if (header == "EndListPersistentRequest") {
    m = ServerMessagePtr( new EndMessage() );
  } else
  if (header == "PersistentRequestRemoved") {
    m = ServerMessagePtr( new PersistentRequestRemovedMessage() );
  } else
  if (header == "PersistentRequestModified") {
    throw new std::runtime_error("Not implemented " + header);
  } else
  if (header == "PutFailed") {
    m = ServerMessagePtr( new PutFailedMessage() );
  } else
  if (header == "GetFailed") {
    m = ServerMessagePtr( new GetFailedMessage() );
  } else
  if (header == "ProtocolError") {
    m = ServerMessagePtr( new ProtocolErrorMessage() );
  } else
  if (header == "IdentifierCollision") {
    m = ServerMessagePtr( new IdentifierCollisionMessage() );
  } else
  if (header == "UnknownNodeIdentifier") {
    m = ServerMessagePtr( new UnknownNodeIdentifierMessage() );
  } else
  if (header == "UnknownPeerNoteType") {
    m = ServerMessagePtr( new UnknownPeerNoteTypeMessage() );
  } else
    throw new std::runtime_error("Not implemented " + header);

  m->message = Message::factory(header);

  m->read(s);

  return m;
}

void ServerMessage::read(Server &s)
{
    static char line[1000];
    for (;;) {
    s.readln(line, 1000);
    line[strlen(line)-1] = 0;

    log().log(DETAIL, "NODE: " + std::string(line));

    if (!strcmp(line, "End") || !strcmp(line, "EndMessage"))
      break;

    char *val = strchr(line, '=');
    *val++ = 0;
    message->setField(std::string(line), std::string(val));
  }
}

const std::string&
ServerMessage::toString() const {
  return message->toString();
}
