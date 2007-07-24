#include <iostream>
#include "FCPLib.h"

using namespace FCPLib;

int main() {
  Node n("List Peers Test", "", -1);
  MessagePtrContainer peers = n.listPeers();
  
  for (MessagePtrContainer::iterator it = peers.begin();
       it != peers.end();
       ++it) {
    std::cout << (*it)->toString() << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
