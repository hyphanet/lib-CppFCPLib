#include <iostream>
#include "FCPLib.h"

using namespace FCPLib;

int main( int argc, char* argv[]) {
  if ( argc != 2 ) {
    std::cout << "Usage :: ./list_peer_notes <identity>" << std::endl;
    return 1;
  } 

  Node n("List Peer Notes Test", "", -1);

  MessagePtrContainer peer_notes = n.listPeerNotes(argv[1]);
  
  for (MessagePtrContainer::iterator it = peer_notes.begin();
       it != peer_notes.end();
       ++it) {
    std::cout << (*it)->toString() << std::endl;
    std::cout << std::endl;
  }
  n.shutdown();

  return 0;
}
