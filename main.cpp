#include <iostream>
#include <string>
#include "Server.h"
#include "Node.h"
#include "FCPMultiMessageResponse.h"
#include "ServerMessage.h"
#include "FCPTestDDAResponse.h"

using namespace std;
using namespace FCPLib;

int main()
{
  Node n("123", "", -1);
  Message::Ptr m = n.getNode();
  std::cout << m->toString() ;

  getchar ();

  ///testDDA
//  Node n("123", "", -1);
//  FCPTestDDAResponse r = n.testDDA("/tmp/", true, true);
//
//  std::cout << r.directory << " " << r.readDirectory << " " << r.writeDirectory << endl;

  /// list pears
//  Node n("123", "", -1);
//  FCPMultiMessageResponse::Ptr r = n.listPeers();
//
//  const std::vector<ServerMessage::Ptr>& mess = r->getMessages();
//  std::vector<ServerMessage::Ptr>::const_iterator it;
//  for (it= mess.begin(); it!=mess.end(); ++it) {
//    cout << (*it)->toString() << "\n";
//  }
//  getchar();
//  r = n.listPeers();
//  getchar();
//  return 0;

  /// put some data
//  Node n("123", "", -1);
//  FCPMultiMessageResponse::Ptr r = n.putData("CHK@", "Hello!", "Ident-1");
//
//  const std::vector<ServerMessage::Ptr>& mess = r->getMessages();
//  std::vector<ServerMessage::Ptr>::const_iterator it;
//  for (it= mess.begin(); it!=mess.end(); ++it) {
//    cout << (*it)->toString() << "\n";
//  }
//  return 0;
}
