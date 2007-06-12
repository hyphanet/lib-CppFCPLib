#include "FCPListPeerNotesResult.h"


using namespace FCPLib;

const std::vector<ServerMessage::ServerMessagePtr>
FCPListPeerNotesResult::getPeerNotes() const
{
  return peerNotes;
}

FCPListPeerNotesResult::FCPListPeerNotesResult(const std::vector<ServerMessage::ServerMessagePtr> &nodeResponse )
{
  // FIX: do not store last message (EndListPeerNotes)
   this->peerNotes = std::vector<ServerMessage::ServerMessagePtr>( nodeResponse );
}
