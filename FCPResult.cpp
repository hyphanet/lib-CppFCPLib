#include "FCPResult.h"
#include <stdexcept>


#include "FCPListPeersResult.h"
#include "FCPClientHelloResult.h"
#include "FCPListPeerNotesResult.h"

using namespace FCPLib;

FCPResult::FCPResultPtr
FCPResult::factory(const std::string cmd, std::vector<ServerMessage::ServerMessagePtr> &nodeResponse)
{
  if (cmd == "ClientHello") {
    return FCPClientHelloResult::FCPClientHelloResultPtr( new FCPClientHelloResult( (*nodeResponse.begin())->getMessage() ) );
  } else
  if (cmd == "ListPeers") {
    return FCPListPeersResult::FCPListPeersResultPtr( new FCPListPeersResult(nodeResponse) );
  } else
  if (cmd == "ListPeerNotes") {
    return FCPListPeerNotesResult::FCPListPeerNotesResultPtr( new FCPListPeerNotesResult(nodeResponse) );
  }

  throw new std::runtime_error("Not implemented : " + cmd);
}

FCPResult::~FCPResult() {}
