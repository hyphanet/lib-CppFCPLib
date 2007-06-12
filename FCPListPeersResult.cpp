#include "FCPListPeersResult.h"


using namespace FCPLib;


const std::vector<ServerMessage::ServerMessagePtr>
FCPListPeersResult::getPeers() const
{
  return peers;
}

FCPListPeersResult::FCPListPeersResult(const std::vector<ServerMessage::ServerMessagePtr> &nodeResponse)
{
  // FIX: do not store last message (EndListPeers)
  this->peers = std::vector<ServerMessage::ServerMessagePtr>( nodeResponse );
}
