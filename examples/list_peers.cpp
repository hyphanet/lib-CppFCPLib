#include <iostream>
#include "FCPLib.h"

using namespace FCPLib;

int main() {
  Node n("List Peers Test", "", -1);
  AdditionalFields fs;

  fs.addField("WithMetadata", true);
  fs.addField("WithVolatile", true);

  MessagePtrContainer peers = n.listPeers(fs);
  
  for (MessagePtrContainer::iterator it = peers.begin();
       it != peers.end();
       ++it) {
    //    std::cout << "Identity :: " << (*it)->getField("identity") << std::endl;
    std::cout << (*it)->toString() << std::endl;
    std::cout << std::endl;
  }
  n.shutdown();

  return 0;
}
