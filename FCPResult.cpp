#include "FCPResult.h"
#include <stdexcept>


#include "FCPListPeersResult.h"

using namespace FCPLib;

FCPResult::FCPResultPtr
FCPResult::factory(const std::string cmd, std::vector<ServerMessage::ServerMessagePtr> nodeResponse)
{
  if (cmd == "ListPeers") {
    return FCPListPeersResult::createFromMessages( nodeResponse );
  }

  throw new std::runtime_error("Not implemented : " + cmd);
}

FCPResult::~FCPResult() {}
