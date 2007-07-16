#ifndef FCPTESTDDAREPLYRESPONSE_H_INCLUDED
#define FCPTESTDDAREPLYRESPONSE_H_INCLUDED

#include "FCPOneMessageResponse.h"
#include <exception>

namespace FCPLib {

class FCPTestDDAReplyResponse : public FCPOneMessageResponse {
  FCPTestDDAReplyResponse(Message::Ptr message_) : FCPOneMessageResponse(message_) {}

  const std::string& getField(std::string field) const {
    const std::string& ret = message->getField(field);
    if (ret == "")
      throw std::logic_error("TestDDAReply does not contain " + field + " field.");
    return ret;
  }
public:
  typedef boost::shared_ptr<FCPTestDDAReplyResponse> Ptr;

  const std::string getDirectory() const throw(std::logic_error){
    return message->getField("Directory");
  }
  const std::string getReadFilename() const throw(std::logic_error){
    return getField("ReadFilename");
  }
  const std::string getWriteFilename() const throw(std::logic_error){
    return getField("WriteFilename");
  }
  const std::string getContent () const throw(std::logic_error){
    return getField("ContentToWrite");
  }

  friend class FCPResult;
};

}

#endif // FCPTESTDDAREPLYRESPONSE_H_INCLUDED
