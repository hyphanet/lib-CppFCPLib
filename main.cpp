#include <iostream>
#include <string>
#include "Server.h"
#include "Node.h"
#include "FCPMultiMessageResponse.h"
#include "ServerMessage.h"

using namespace std;
using namespace FCPLib;

int main()
{
//  char line[100];
//	Server s;
//	s.send("ClientHello\nName=123\nExpectedVersion=2.0\nEndMessage\n");
//
//	for(;;){
//	  s.readln(line, 100);
//	  cout << line;
//	}

  /// list pears
//  Node n("123", "", -1);
//  FCPMultiMessageResponse::FCPMultiMessageResponsePtr r = n.listPeers();
//
//  const std::vector<ServerMessage::ServerMessagePtr>& mess = r->getMessages();
//  std::vector<ServerMessage::ServerMessagePtr>::const_iterator it;
//  for (it= mess.begin(); it!=mess.end(); ++it) {
//    cout << (*it)->toString() << "\n";
//  }
//  return 0;

  /// put some data
  Node n("123", "", -1);
  FCPMultiMessageResponse::FCPMultiMessageResponsePtr r = n.putData("CHK@", "Hello!", "Ident-1");

  const std::vector<ServerMessage::ServerMessagePtr>& mess = r->getMessages();
  std::vector<ServerMessage::ServerMessagePtr>::const_iterator it;
  for (it= mess.begin(); it!=mess.end(); ++it) {
    cout << (*it)->toString() << "\n";
  }
  return 0;
}
