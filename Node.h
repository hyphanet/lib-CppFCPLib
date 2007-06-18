#ifndef NODE_H__
#define NODE_H__

#include <string>
#include <memory>
#include <map>

#include "zthread/Thread.h"
#include "zthread/ThreadedExecutor.h"

#include "TQueue.h"
#include "NodeThread.h"
#include "FCPMultiMessageResponse.h"
#include "FCPOneMessageResponse.h"

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

  FCPMultiMessageResponse::FCPMultiMessageResponsePtr listPeers(bool, bool);
  FCPMultiMessageResponse::FCPMultiMessageResponsePtr listPeerNotes(const std::string&);
  void addPeer(const std::string &, bool isURL);
  void addPeer(const std::map<std::string, std::string> &message);
  FCPOneMessageResponse::FCPOneMessageResponsePtr modifyPeer(const std::string &, bool, bool, bool);
  FCPOneMessageResponse::FCPOneMessageResponsePtr modifyPeerNote(const std::string &, const std::string &, int);
  FCPOneMessageResponse::FCPOneMessageResponsePtr removePeer(const std::string &);
  FCPOneMessageResponse::FCPOneMessageResponsePtr getNode(bool, bool);
};
}

#endif
