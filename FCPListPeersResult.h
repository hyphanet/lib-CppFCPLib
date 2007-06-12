#ifndef FCPLISTPEERSRESULT_H__
#define FCPLISTPEERSRESULT_H__


#include <vector>
#include <memory>
#include "Message.h"
#include "FCPResult.h"


namespace FCPLib {


class FCPListPeersResult : public FCPResult {
  std::vector<ServerMessage::ServerMessagePtr> peers;

  FCPListPeersResult( const std::vector<ServerMessage::ServerMessagePtr> &nodeResponse );
public:
  typedef boost::shared_ptr<FCPListPeersResult> FCPListPeersResultPtr;

  const std::vector<ServerMessage::ServerMessagePtr> getPeers() const;
  friend class FCPResult;
};
}

#endif
