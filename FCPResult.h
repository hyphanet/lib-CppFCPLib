#ifndef FCPRESULT_H__
#define FCPRESULT_H__

#include <vector>
#include <string>
#include <algorithm>

#include "JobTicket.h"

namespace FCPLib {

class TestDDAReplyResponse {
  Message::Ptr message;

  TestDDAReplyResponse(Message::Ptr message_) : message(message_) {}

  const std::string& getField(std::string field) const {
    const std::string& ret = message->getField(field);
    if (ret == "")
      throw std::logic_error("TestDDAReply does not contain " + field + " field.");
    return ret;
  }
public:
  typedef boost::shared_ptr<TestDDAReplyResponse> Ptr;

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

  friend struct TestDDAReplyConverter;
};

class TestDDAResponse {
public:
  TestDDAResponse(std::string dir, bool read, bool write)
    : directory(dir),
      readDirectory(read),
      writeDirectory(write)
  {}
  std::string directory;
  bool readDirectory;
  bool writeDirectory;
};

///////////

Message::Ptr
getMessage(ServerMessage::Ptr sm) {
  return Message::Ptr( sm->getMessage() );
}

struct MessageConverter {
  Message::Ptr
  operator()( Response &resp )
  {
    return Message::Ptr( resp.responses.front()->getMessage() );
  }
};

struct LastMessageConverter {
  ServerMessage::Ptr
  operator()( Response &resp )
  {
    return ServerMessage::Ptr( resp.responses.back() );
  }
};

struct VectorConverter {
  std::vector<Message::Ptr>
  operator()( Response &resp )
  {
    std::vector<Message::Ptr> ret = std::vector<Message::Ptr>( resp.responses.size() );
    std::transform(resp.responses.begin(), resp.responses.end(), ret.begin(), getMessage);
    return ret;
  }
};

struct VectorWithoutLastConverter {
  std::vector<Message::Ptr>
  operator()( Response &resp )
  {
    int numToCopy = resp.responses.size();
    std::vector<Message::Ptr> ret = std::vector<Message::Ptr>( numToCopy );
    std::transform(resp.responses.begin(), resp.responses.begin() + numToCopy, ret.begin(), getMessage);
    return ret;
  }
};

struct TestDDAReplyConverter {
  TestDDAReplyResponse::Ptr
  operator()( Response &resp )
  {
    return TestDDAReplyResponse::Ptr( new TestDDAReplyResponse(resp.responses.front()->getMessage()) );
  }
};

//////////

template<typename ReturnT, typename ConverterT>
ReturnT createResult( Response &resp )
{
  return ConverterT()( resp );
}

}

#endif
