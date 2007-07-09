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
  } else
  if (cmd == "ModifyPeer") {
    return FCPOneMessageResponse::FCPOneMessageResponsePtr( new FCPOneMessageResponse( (*nodeResponse.begin())->getMessage() ) );
  } else
  if (cmd == "ModifyPeerNote") {
    return FCPOneMessageResponse::FCPOneMessageResponsePtr( new FCPOneMessageResponse( (*nodeResponse.begin())->getMessage() ) );
  } else
  if (cmd == "RemovePeer") {
    return FCPOneMessageResponse::FCPOneMessageResponsePtr( new FCPOneMessageResponse( (*nodeResponse.begin())->getMessage() ) );
  } else
  if (cmd == "GetNode") {
    return FCPOneMessageResponse::FCPOneMessageResponsePtr( new FCPOneMessageResponse( (*nodeResponse.begin())->getMessage() ) );
  } else
  if (cmd == "GetConfig") {
    return FCPOneMessageResponse::FCPOneMessageResponsePtr( new FCPOneMessageResponse( (*nodeResponse.begin())->getMessage() ) );
  } else
  if (cmd == "ClientPut") {
    return FCPMultiMessageResponse::FCPMultiMessageResponsePtr( new FCPMultiMessageResponse(nodeResponse, false) );
  }

  throw new std::runtime_error("Not implemented : " + cmd);
}

FCPResult::~FCPResult() {}
