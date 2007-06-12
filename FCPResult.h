#ifndef FCPRESULT_H__
#define FCPRESULT_H__

#include "ServerMessage.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

namespace FCPLib {

class FCPResult {
public:
  typedef boost::shared_ptr<FCPResult> FCPResultPtr;

  static FCPResultPtr factory(const std::string cmd, std::vector<ServerMessage::ServerMessagePtr> &nodeResponse);
  virtual ~FCPResult() = 0;
};

}

#endif
