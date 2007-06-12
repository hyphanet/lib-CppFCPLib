
#include "ServerMessage.h"
#include "PeerNoteMessage.h"
#include "PeerMessage.h"
#include "NodeHelloMessage.h"
#include "Log.h"

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
  }
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
