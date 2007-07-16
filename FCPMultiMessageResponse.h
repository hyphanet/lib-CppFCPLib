#ifndef FCPMULTIMESSAGERESPONSE_H__
#define FCPMULTIMESSAGERESPONSE_H__


#include <vector>
#include <memory>
#include "Message.h"
#include "FCPResult.h"


namespace FCPLib {


class FCPMultiMessageResponse : public FCPResult {

protected:
  std::vector<ServerMessage::Ptr> messages;

  FCPMultiMessageResponse( const std::vector<ServerMessage::Ptr> &nodeResponse, bool removeLast = true ) {
    messages = nodeResponse;
    if (removeLast)
      messages.pop_back();
  }
public:
  typedef boost::shared_ptr<FCPMultiMessageResponse> Ptr;

  const std::vector<ServerMessage::Ptr>& getMessages() const {
    return messages;
  }
  friend class FCPResult;
};
}

#endif // FCPMULTIMESSAGERESPONSE_H__
