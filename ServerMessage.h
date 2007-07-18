#ifndef SERVERMESSAGE_H__
#define SERVERMESSAGE_H__

#include "Message.h"
#include "Server.h"
#include <exception>
#include <string>

namespace FCPLib {

class ServerMessage {
  void read(boost::shared_ptr<Server> s);

protected:
  Message::Ptr message;

public:
  typedef boost::shared_ptr<ServerMessage> Ptr;
  static Ptr factory(boost::shared_ptr<Server> s);

  virtual std::string getIdOfJob() const = 0;
  virtual bool isLast(const std::string &cmd) const = 0;
  virtual bool isError() const = 0;
  virtual ~ServerMessage() {}

  const std::string& toString() const;
  const Message::Ptr getMessage() const { return message; }
};

typedef std::vector<ServerMessage::Ptr> Response;


template<bool isLast>
struct IsLastT {
  bool operator()(const std::string &cmd) const {
    return isLast;
  };
};
typedef IsLastT<true> IsLastTrue;
typedef IsLastT<false> IsLastFalse;

struct IsLastPeer {
  bool operator()(const std::string &cmd) const {
    if (cmd == "ListPeers")
      return false;
    else if (cmd == "ModifyPeer")
      return true;
    else if (cmd == "AddPeer")
      return true;
    else if (cmd == "ListPeer")
      return true;
    //TODO: check if this is all
    throw std::runtime_error("Unknown command");
  }
};

struct IsLastPeerNote {
  bool operator()(const std::string &cmd) const {
    if (cmd == "ListPeerNotes")
      return false;
    else if (cmd == "ModifyPeerNote")
      return true;
    //TODO: check if this is all
    throw std::runtime_error("Unknown command");
  }
};


struct GlobalIdOfJob {
  inline std::string operator()(const std::string& id){
    return "__global";
  }
};

struct IdentifierIdOfJob {
  inline std::string operator()(const std::string& id){
    return  id.size() ? id : "";
  }
};

template<typename IdOfJobT, typename isLastT = IsLastTrue, bool isErrorT = false>
class ServerMessageT : public ServerMessage {
  ServerMessageT() {}
public:
  inline std::string getIdOfJob() const {
    return IdOfJobT()(message->getField("Identifier"));
  }
  bool isLast(const std::string &cmd) const {
    return isLastT()( cmd );
  }
  bool isError() const {
    return isErrorT;
  }
  friend class ServerMessage;
};

typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, false> NodeHelloMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, true> CloseConnectionDuplicateNameMessage;

typedef class ServerMessageT<GlobalIdOfJob, IsLastPeer, false> PeerMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastPeerNote, false> PeerNoteMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, false> PeerRemovedMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, false> EndMessage;

typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, false> NodeDataMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, false> ConfigDataMessage;

typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, false> TestDDAReplyMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, false> TestDDACompleteMessage;

typedef class ServerMessageT<IdentifierIdOfJob, IsLastTrue, false> SSKKeypairMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastFalse, false> URIGeneratedMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastTrue, false> PutSuccessfulMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastFalse, false> StartedCompressionMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastFalse, false> SimpleProgressMessage;

typedef class ServerMessageT<IdentifierIdOfJob, IsLastFalse, false> FinishedCompressionMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastFalse, false> PersistentRequestRemovedMessage; // ????

typedef class ServerMessageT<IdentifierIdOfJob, IsLastTrue, true> PutFailedMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastTrue, true> GetFailedMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastTrue, true> ProtocolErrorMessage;
typedef class ServerMessageT<IdentifierIdOfJob, IsLastTrue, true> IdentifierCollisionMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, true> UnknownNodeIdentifierMessage;
typedef class ServerMessageT<GlobalIdOfJob, IsLastTrue, true> UnknownPeerNoteTypeMessage;



}

#endif // SERVERMESSAGE_H__
