#ifndef NODE_H__
#define NODE_H__

#include <string>
#include <memory>

#include "zthread/Thread.h"
#include "zthread/ThreadedExecutor.h"

#include "TQueue.h"
#include "NodeThread.h"
#include "FCPListPeersResult.h"

namespace FCPLib {
class Node {
  std::string name;
  ZThread::CountedPtr< JobTicketQueue > clientReqQueue;
  NodeThread *nodeThread;
  ZThread::ThreadedExecutor executor;

  static std::string _getUniqueId();

public:
  Node(std::string name, std::string host, int port);
  ~Node();

  FCPListPeersResult listPeers(bool, bool);
};
}

#endif
