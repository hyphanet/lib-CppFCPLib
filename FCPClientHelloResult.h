#ifndef FCPCLIENTHELLORESULT_H__
#define FCPCLIENTHELLORESULT_H__

#include <vector>
#include <memory>
#include "Message.h"
#include "FCPResult.h"

namespace FCPLib {

class FCPClientHelloResult : public FCPResult {
  Message::MessagePtr message;

  FCPClientHelloResult(Message::MessagePtr message_) : message(message_) {}
public:
  typedef boost::shared_ptr<FCPClientHelloResult> FCPClientHelloResultPtr;

  const Message::MessagePtr getMessage() const { return message; }

  friend class FCPResult;
};
}

#endif // FCPCLIENTHELLORESULT_H__
