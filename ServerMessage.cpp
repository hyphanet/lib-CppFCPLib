
#include "ServerMessage.h"
#include "PeerMessage.h"

using namespace FCPLib;


ServerMessage::ServerMessagePtr
ServerMessage::factory(Server &s){
  ServerMessage::ServerMessagePtr m;
  static char line[1000];

  s.readln(line, 1000);
  line[strlen(line)-1] = 0;
  std::string header = std::string(line);

  if (header == "Peer") {
    m = ServerMessagePtr( new PeerMessage() );
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

    if (!strcmp(line, "End") || !strcmp(line, "EndMessage"))
      break;

    char *val = strchr(line, '=');
    *val++ = 0;
    message->setField(std::string(line), std::string(val));
  }
}


