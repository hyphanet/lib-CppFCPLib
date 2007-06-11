#include "FCPListPeersResult.h"


using namespace FCPLib;

FCPListPeersResult::FCPResultPtr
FCPListPeersResult::createFromMessages(const std::vector<ServerMessage::ServerMessagePtr> &nodeResponse)
{
  FCPListPeersResultPtr ret( new FCPListPeersResult() );

  ret->peers = std::vector<ServerMessage::ServerMessagePtr>( nodeResponse );

  return ret;
}
