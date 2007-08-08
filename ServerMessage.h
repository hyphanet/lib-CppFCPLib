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
  virtual void read(boost::shared_ptr<Server> s);

protected:
  Message::Ptr message;

public:
  typedef boost::shared_ptr<ServerMessage> Ptr;
  static Ptr factory(boost::shared_ptr<Server> s);

  std::string getIdOfJob() const {
    std::string id = message->getField("Identifier");
    return  id.size() ? id : "";
  }
  virtual bool isLast(const JobTicketPtr job) const = 0;
  virtual bool isError() const = 0;
  virtual ~ServerMessage() {}

  const std::string& toString() const;
  const Message::Ptr getMessage() const { return message; }
};

typedef std::vector<ServerMessage::Ptr> Response;


template<bool isLast>
struct IsLastT {
  Message::Ptr message;
  IsLastT( Message::Ptr m ) : message(m) {}
  bool operator()(const JobTicketPtr job) const {
    return isLast;
  }
};
typedef IsLastT<true> IsLastTrue;
typedef IsLastT<false> IsLastFalse;

struct IsLastPeer {
  Message::Ptr message;
  IsLastPeer( Message::Ptr m ) : message(m) {}
  bool operator()(const JobTicketPtr job) const;
};

struct IsLastPeerNote {
  Message::Ptr message;
  IsLastPeerNote( Message::Ptr m ) : message(m) {}
  bool operator()(const JobTicketPtr job) const;
};

struct IsLastPutFailed {
  Message::Ptr message;
  IsLastPutFailed( Message::Ptr m ) : message(m) {}
  bool operator()(const JobTicketPtr job) const;
};

struct IsLastGetFailed {
  Message::Ptr message;
  IsLastGetFailed( Message::Ptr m ) : message(m) {}
  bool operator()(const JobTicketPtr job) const;
};

struct IsLastDataFound {
  Message::Ptr message;
  IsLastDataFound( Message::Ptr m ) : message(m) {}
  bool operator()(const JobTicketPtr job) const;
};

template<typename isLastT = IsLastTrue, bool isErrorT = false>
class ServerMessageT : public ServerMessage {
  friend class ServerMessage;

  ServerMessageT() {}

public:
  bool isLast(const JobTicketPtr job) const {
    return isLastT( message )( job );
  }
  bool isError() const {
    return isErrorT;
  }
};

class AllDataMessage : public ServerMessage {
  friend class ServerMessage;

  boost::shared_ptr<Server> server;
  std::size_t bytesToRead;

  void read(boost::shared_ptr<Server> s);

  AllDataMessage() {}

public:
  bool isLast(const JobTicketPtr job) const;
  bool isError() const { return false; }
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
typedef class ServerMessageT<IsLastFalse, false> PutFetchableMessage;
typedef class ServerMessageT<IsLastDataFound, false> DataFoundMessage;

typedef class ServerMessageT<IsLastFalse, false> StartedCompressionMessage;
typedef class ServerMessageT<IsLastFalse, false> FinishedCompressionMessage;
typedef class ServerMessageT<IsLastFalse, false> SimpleProgressMessage;
typedef class ServerMessageT<IsLastTrue, false> PersistentRequestRemovedMessage;
typedef class ServerMessageT<IsLastFalse, false> PersistentRequestModifiedMessage;

typedef class ServerMessageT<IsLastPutFailed, true> PutFailedMessage;
typedef class ServerMessageT<IsLastGetFailed, true> GetFailedMessage;
typedef class ServerMessageT<IsLastTrue, true> ProtocolErrorMessage;
typedef class ServerMessageT<IsLastTrue, true> IdentifierCollisionMessage;
typedef class ServerMessageT<IsLastTrue, true> UnknownNodeIdentifierMessage;
typedef class ServerMessageT<IsLastTrue, true> UnknownPeerNoteTypeMessage;

typedef class ServerMessageT<IsLastFalse, false> SubscribedUSKUpdate;

}

#endif // SERVERMESSAGE_H__
