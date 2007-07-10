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
#include "AdditionalFields.h"

namespace FCPLib {
class Node {
  std::string name;
  ZThread::CountedPtr< JobTicketQueue > clientReqQueue;
  NodeThread *nodeThread;
  ZThread::ThreadedExecutor executor;

  static std::string _getUniqueId();

  void checkProtocolError(JobTicket::JobTicketPtr &job);
  Message::MessagePtr nodeHelloMessage;
public:
  Node(std::string name, std::string host, int port);
  ~Node();

  const Message::MessagePtr getNodeHelloMessage() const;

  FCPMultiMessageResponse::FCPMultiMessageResponsePtr listPeers(const AdditionalFields& = AdditionalFields());
  FCPMultiMessageResponse::FCPMultiMessageResponsePtr listPeerNotes(const std::string&);
  void addPeer(const std::string &, bool isURL);
  void addPeer(const std::map<std::string, std::string> &message);
  FCPOneMessageResponse::FCPOneMessageResponsePtr modifyPeer(const std::string &, const AdditionalFields& = AdditionalFields());
  FCPOneMessageResponse::FCPOneMessageResponsePtr modifyPeerNote(const std::string &, const std::string &, int);
  FCPOneMessageResponse::FCPOneMessageResponsePtr removePeer(const std::string &);
  FCPOneMessageResponse::FCPOneMessageResponsePtr getNode(const AdditionalFields& = AdditionalFields());
  FCPOneMessageResponse::FCPOneMessageResponsePtr getConfig(const AdditionalFields& = AdditionalFields());
  FCPOneMessageResponse::FCPOneMessageResponsePtr modifyConfig(Message::MessagePtr m);
  FCPTestDDAReplyResponse::FCPTestDDAReplyResponsePtr testDDARequest(std::string dir, bool read, bool write);
  FCPOneMessageResponse::FCPOneMessageResponsePtr  testDDAResponse(std::string dir, std::string readContent = "");
  FCPOneMessageResponse::FCPOneMessageResponsePtr generateSSK(std::string identifier);
  FCPMultiMessageResponse::FCPMultiMessageResponsePtr putData(const std::string , // URI
                                                              const std::string , // Data
                                                              const std::string = "", // Identifier
                                                              const AdditionalFields& = AdditionalFields()
                                                              );
  FCPMultiMessageResponse::FCPMultiMessageResponsePtr putRedirect(const std::string , // URI
                                                                  const std::string , // Target
                                                                  const std::string = "", // Identifier
                                                                  const AdditionalFields& = AdditionalFields()
                                                                  );
};
}

#endif
