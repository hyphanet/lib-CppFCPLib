#include "FCPResult.h"
#include <stdexcept>


#include "FCPMultiMessageResponse.h"
#include "FCPOneMessageResponse.h"
#include "FCPTestDDAReplyResponse.h"
#include "FCPErrorResponse.h"

using namespace FCPLib;

FCPResult::Ptr
FCPResult::factory(const std::string cmd, Response &nodeResponse)
{
  // check for error
  if (nodeResponse.responses.back()->isError())
    return FCPErrorResponse::Ptr( new FCPErrorResponse(nodeResponse.responses) );

  if (cmd == "ClientHello") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "ListPeers") {
    return FCPMultiMessageResponse::Ptr( new FCPMultiMessageResponse(nodeResponse.responses) );
  } else
  if (cmd == "ListPeerNotes") {
    return FCPMultiMessageResponse::Ptr( new FCPMultiMessageResponse(nodeResponse.responses) );
  } else
  if (cmd == "ModifyPeer") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "ModifyPeerNote") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "RemovePeer") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "GetNode") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "GetConfig") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "ModifyConfig") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "TestDDARequest") {
    return FCPTestDDAReplyResponse::Ptr( new FCPTestDDAReplyResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "TestDDAResponse") {
    return FCPOneMessageResponse::Ptr( new FCPTestDDAReplyResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "GenerateSSK") {
    return FCPOneMessageResponse::Ptr( new FCPOneMessageResponse( (*nodeResponse.responses.begin())->getMessage() ));
  } else
  if (cmd == "ClientPut") {
    return FCPMultiMessageResponse::Ptr( new FCPMultiMessageResponse(nodeResponse.responses, false) );
  }

  throw std::runtime_error("Not implemented : " + cmd);
}

FCPResult::~FCPResult() {}
