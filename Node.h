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
#include "FCPTestDDAReplyResponse.h"
#include "FCPTestDDAResponse.h"
#include "AdditionalFields.h"

namespace FCPLib {
class Node {
  std::string name;
  ZThread::CountedPtr< JobTicketQueue > clientReqQueue;
  NodeThread* nodeThread;
  ZThread::ThreadedExecutor executor;

  int globalCommandsTimeout;

  static std::string _getUniqueId();

  void checkProtocolError(Response &resp);
  Message::Ptr nodeHelloMessage;
public:
  Node(std::string name, std::string host, int port);
  ~Node();

  int getGlobalCommandsTimeout() const {
    return globalCommandsTimeout;
  }

  Node& setGlobalCommandsTimeout(int t) {
    globalCommandsTimeout = t; return *this;
  }

  bool isAlive() const {
    return nodeThread->isAlive();
  }

  std::exception getFailure() const {
    return *nodeThread->getFailure();
  }

  const Message::Ptr getNodeHelloMessage() const;

  FCPMultiMessageResponse::Ptr listPeers(const AdditionalFields& = AdditionalFields());
  FCPMultiMessageResponse::Ptr listPeerNotes(const std::string&);
  FCPOneMessageResponse::Ptr addPeer(const std::string &, bool isURL);
  FCPOneMessageResponse::Ptr addPeer(const std::map<std::string, std::string> &message);
  FCPOneMessageResponse::Ptr modifyPeer(const std::string &, const AdditionalFields& = AdditionalFields());
  FCPOneMessageResponse::Ptr modifyPeerNote(const std::string &, const std::string &, int);
  FCPOneMessageResponse::Ptr removePeer(const std::string &);
  Message::Ptr getNode(const AdditionalFields& = AdditionalFields());
  FCPOneMessageResponse::Ptr getConfig(const AdditionalFields& = AdditionalFields());
  FCPOneMessageResponse::Ptr modifyConfig(Message::Ptr m);

  FCPTestDDAReplyResponse::Ptr testDDARequest(std::string dir, bool read, bool write);
  FCPTestDDAResponse testDDAResponse(std::string dir, std::string readContent = "");
  FCPTestDDAResponse testDDA(std::string dir, bool read, bool write);

  FCPOneMessageResponse::Ptr generateSSK(std::string identifier);
  JobTicket::Ptr putData(const std::string , // URI
                                  std::istream*, // Data Stream
                                  int, // dataLength
                                  const std::string = "", // Identifier
                                  const AdditionalFields& = AdditionalFields()
                                  );
  JobTicket::Ptr putRedirect(const std::string , // URI
                                      const std::string , // Target
                                      const std::string = "", // Identifier
                                      const AdditionalFields& = AdditionalFields()
                                      );
};
}

#endif
