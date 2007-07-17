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

struct GetMessage {
  Message::Ptr
  operator()( ServerMessage::Ptr sm ){
    return sm->getMessage();
  }
};

struct MessageConverter {
  Message::Ptr
  operator()( Response &resp )
  {
    return Message::Ptr( resp.front()->getMessage() );
  }
};

struct LastMessageConverter {
  ServerMessage::Ptr
  operator()( Response &resp )
  {
    return ServerMessage::Ptr( resp.back() );
  }
};

struct VectorConverter {
  MessagePtrContainer
  operator()( Response &resp )
  {
    MessagePtrContainer ret( resp.size() ) ;
    transform( resp.begin(), resp.end(), ret.begin(), GetMessage() );
    return ret;
  }
};

struct VectorWithoutLastConverter {
  MessagePtrContainer
  operator()( Response &resp )
  {
    int numToCopy = resp.size() - 1;
    MessagePtrContainer ret = MessagePtrContainer( numToCopy );
    transform( resp.begin(), resp.begin() + numToCopy, ret.begin(), GetMessage() );
    return ret;
  }
};

struct TestDDAReplyConverter {
  TestDDAReplyResponse::Ptr
  operator()( Response &resp )
  {
    return TestDDAReplyResponse::Ptr( new TestDDAReplyResponse(resp.front()->getMessage()) );
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
