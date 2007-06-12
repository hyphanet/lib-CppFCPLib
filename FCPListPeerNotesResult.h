#ifndef FCPLISTPEERNOTESRESULT_H__
#define FCPLISTPEERNOTESRESULT_H__

#include <vector>
#include <memory>
#include "Message.h"
#include "FCPResult.h"


namespace FCPLib {


class FCPListPeerNotesResult : public FCPResult {
  std::vector<ServerMessage::ServerMessagePtr> peerNotes;

  FCPListPeerNotesResult(const std::vector<ServerMessage::ServerMessagePtr> &nodeResponse );
public:
  typedef boost::shared_ptr<FCPListPeerNotesResult> FCPListPeerNotesResultPtr;

  const std::vector<ServerMessage::ServerMessagePtr> getPeerNotes() const;

  friend class FCPResult;
};
}

#endif // FCPLISTPEERNOTESRESULT_H__
