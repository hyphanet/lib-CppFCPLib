

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include "Server.h"

namespace FCPLib {

class Message {
  std::string repr;
  std::string header;

  std::map<std::string, std::string> fields;

  bool isDataType;
  bool isReprValid;

  Message();
public:
  typedef boost::shared_ptr<Message> MessagePtr;
  static MessagePtr factory(std::string &header);
  static MessagePtr factory(const char *header);
  static MessagePtr factory(Server &s);

  void setField(std::string key, std::string value);
  inline std::string getField(const std::string &key);
  const std::string& getHeader() const;

  const std::string& toString();
};

}

#endif


