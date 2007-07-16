#ifndef FCPRESULT_H__
#define FCPRESULT_H__

#include <vector>
#include <string>
#include <algorithm>

#include "JobTicket.h"

namespace FCPLib {

class FCPResult {
public:
  typedef boost::shared_ptr<FCPResult> Ptr;

  static Ptr factory(const std::string cmd, Response &nodeResponse);
  virtual ~FCPResult() = 0;
};


//Message::Ptr getMessage(ServerMessage::Ptr sm) {
//  return Message::Ptr( sm->getMessage() );
//}

Message::Ptr
MessageConverter( Response &resp )
{
  return Message::Ptr( resp.responses.front()->getMessage() );
}

ServerMessage::Ptr
LastMessage( Response &resp )
{
  return ServerMessage::Ptr( resp.responses.back() );
}

//std::vector<Message::Ptr>
//VectorConverter( Response &resp )
//{
//  std::vector<Message::Ptr> ret = std::vector<Message::Ptr>( nodeResponse.size() );
//  std::transform(nodeResponse.begin(), nodeResponse.end(), ret.begin(), getMessage);
//  return ret;
//}

template<class ReturnT, class ConverterT >
ReturnT createResult( Response &resp )
{
  return ConverterT( resp );
}

}

#endif
