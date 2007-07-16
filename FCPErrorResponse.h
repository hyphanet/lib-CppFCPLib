#ifndef FCPERRORRESPONSE_H_INCLUDED
#define FCPERRORRESPONSE_H_INCLUDED

#include <vector>
#include <memory>
#include "Message.h"
#include "FCPResult.h"


namespace FCPLib {


class FCPErrorResponse : public FCPResult {

protected:
  std::vector<ServerMessage::Ptr> messages;

  FCPErrorResponse( const std::vector<ServerMessage::Ptr>& nodeResponse)
    : messages(nodeResponse)
  {
  }
public:
  typedef boost::shared_ptr<FCPErrorResponse> Ptr;

  const std::vector<ServerMessage::Ptr>& getMessages() const {
    return messages;
  }
  friend class FCPResult;
};
}

#endif // FCPERRORRESPONSE_H_INCLUDED
