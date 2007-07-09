

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace FCPLib {

class Message {
  std::string repr;
  std::string header;

  std::map<std::string, std::string> fields;

protected:
  bool isReprValid;
  bool isDataType;

  Message() :
    isReprValid(false),
    isDataType(false)
  {}

public:
  typedef boost::shared_ptr<Message> MessagePtr;

  static Message::MessagePtr factory(std::string header, bool isData=false);

  void setField(std::string key, std::string value);
  void setFields(const std::map<std::string, std::string> &fields);
  const std::string& getField(std::string key) const;
  const std::string& getHeader() const;

  const std::string& toString();
};

class DataMessage : public Message {
protected:
  DataMessage() { isDataType = true;}

  friend class Message;
};

}

#endif


