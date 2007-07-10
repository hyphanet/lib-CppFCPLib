#ifndef FCPERRORRESPONSE_H_INCLUDED
#define FCPERRORRESPONSE_H_INCLUDED

#include <vector>
#include <memory>
#include "Message.h"
#include "FCPResult.h"


namespace FCPLib {


class FCPErrorResponse : public FCPResult {

protected:
  std::vector<ServerMessage::ServerMessagePtr> messages;

  FCPErrorResponse( const std::vector<ServerMessage::ServerMessagePtr> &nodeResponse)
    : messages(nodeResponse)
  {
  }
public:
  typedef boost::shared_ptr<FCPErrorResponse> FCPErrorResponsePtr;

  const std::vector<ServerMessage::ServerMessagePtr>& getMessages() const {
    return messages;
  }
  friend class FCPResult;
};
}

#endif // FCPERRORRESPONSE_H_INCLUDED
