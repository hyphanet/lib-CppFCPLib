#ifndef FCPONEMESSAGERESPONSE_H__
#define FCPONEMESSAGERESPONSE_H__

#include <vector>
#include <memory>
#include "Message.h"
#include "FCPResult.h"

namespace FCPLib {

class FCPOneMessageResponse : public FCPResult {
protected:
  Message::Ptr message;
  FCPOneMessageResponse(Message::Ptr message_) : message(message_) {}
public:
  typedef boost::shared_ptr<FCPOneMessageResponse> Ptr;

  const Message::Ptr getMessage() const { return message; }

  friend class FCPResult;
};
}

#endif // FCPONEMESSAGERESPONSE_H__
