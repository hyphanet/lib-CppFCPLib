#ifndef SERVERMESSAGE_H__
#define SERVERMESSAGE_H__

#include <string>
#include <boost/shared_ptr.hpp>

#include "Message.h"
#include "Server.h"
#include "Exceptions.h"

namespace FCPLib {

class JobTicket;
typedef boost::shared_ptr<JobTicket> JobTicketPtr;

class ServerMessage {
  void read(boost::shared_ptr<Server> s);

protected:
  Message::Ptr message;

public:
  typedef boost::shared_ptr<ServerMessage> Ptr;
  static Ptr factory(boost::shared_ptr<Server> s);

  virtual std::string getIdOfJob() const = 0;
  virtual bool isLast(const JobTicketPtr job) const = 0;
  virtual bool isError() const = 0;
  virtual ~ServerMessage() {}

  const std::string& toString() const;
  const Message::Ptr getMessage() const { return message; }
};

typedef std::vector<ServerMessage::Ptr> Response;


template<bool isLast>
struct IsLastT {
  bool operator()(const JobTicketPtr job) const {
    return isLast;
  }
};
typedef IsLastT<true> IsLastTrue;
typedef IsLastT<false> IsLastFalse;

struct IsLastPeer {
  bool operator()(const JobTicketPtr job) const;
};

struct IsLastPeerNote {
  bool operator()(const JobTicketPtr job) const;
};

template<typename isLastT = IsLastTrue, bool isErrorT = false>
class ServerMessageT : public ServerMessage {
  ServerMessageT() {}
public:
  inline std::string getIdOfJob() const {
    std::string id = message->getField("Identifier");
    return  id.size() ? id : "";
  }
  bool isLast(const JobTicketPtr job) const {
    return isLastT()( job );
  }
  bool isError() const {
    return isErrorT;
  }
  friend class ServerMessage;
};

typedef class ServerMessageT<IsLastTrue, false> NodeHelloMessage;
typedef class ServerMessageT<IsLastTrue, true> CloseConnectionDuplicateNameMessage;

typedef class ServerMessageT<IsLastPeer, false> PeerMessage;
typedef class ServerMessageT<IsLastPeerNote, false> PeerNoteMessage;
typedef class ServerMessageT<IsLastTrue, false> PeerRemovedMessage;
typedef class ServerMessageT<IsLastTrue, false> EndMessage;

typedef class ServerMessageT<IsLastTrue, false> NodeDataMessage;
typedef class ServerMessageT<IsLastTrue, false> ConfigDataMessage;

typedef class ServerMessageT<IsLastTrue, false> TestDDAReplyMessage;
typedef class ServerMessageT<IsLastTrue, false> TestDDACompleteMessage;

typedef class ServerMessageT<IsLastTrue, false> SSKKeypairMessage;
typedef class ServerMessageT<IsLastFalse, false> PersistentGetMessage;
typedef class ServerMessageT<IsLastFalse, false> PersistentPutMessage;
typedef class ServerMessageT<IsLastFalse, false> PersistentPutDirMessage;

typedef class ServerMessageT<IsLastFalse, false> URIGeneratedMessage;
typedef class ServerMessageT<IsLastTrue, false> PutSuccessfulMessage;
typedef class ServerMessageT<IsLastFalse, false> StartedCompressionMessage;
typedef class ServerMessageT<IsLastFalse, false> SimpleProgressMessage;

typedef class ServerMessageT<IsLastFalse, false> FinishedCompressionMessage;
typedef class ServerMessageT<IsLastFalse, false> PersistentRequestRemovedMessage; // ????

typedef class ServerMessageT<IsLastTrue, true> PutFailedMessage;
typedef class ServerMessageT<IsLastTrue, true> GetFailedMessage;
typedef class ServerMessageT<IsLastTrue, true> ProtocolErrorMessage;
typedef class ServerMessageT<IsLastTrue, true> IdentifierCollisionMessage;
typedef class ServerMessageT<IsLastTrue, true> UnknownNodeIdentifierMessage;
typedef class ServerMessageT<IsLastTrue, true> UnknownPeerNoteTypeMessage;

typedef class ServerMessageT<IsLastFalse, false> SubscribedUSKUpdate;

}

#endif // SERVERMESSAGE_H__
