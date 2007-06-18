#include "FCPResult.h"
#include <stdexcept>


#include "FCPMultiMessageResponse.h"
#include "FCPOneMessageResponse.h"

using namespace FCPLib;

FCPResult::FCPResultPtr
FCPResult::factory(const std::string cmd, std::vector<ServerMessage::ServerMessagePtr> &nodeResponse)
{
  if (cmd == "ClientHello") {
    return FCPOneMessageResponse::FCPOneMessageResponsePtr( new FCPOneMessageResponse( (*nodeResponse.begin())->getMessage() ) );
  } else
  if (cmd == "ListPeers") {
    return FCPMultiMessageResponse::FCPMultiMessageResponsePtr( new FCPMultiMessageResponse(nodeResponse) );
  } else
  if (cmd == "ListPeerNotes") {
    return FCPMultiMessageResponse::FCPMultiMessageResponsePtr( new FCPMultiMessageResponse(nodeResponse) );
  }

  throw new std::runtime_error("Not implemented : " + cmd);
}

FCPResult::~FCPResult() {}
