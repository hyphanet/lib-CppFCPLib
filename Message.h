

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <map>
#include <vector>
#include <ostream>
#include <istream>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace FCPLib {

class Message {
protected:
  std::string repr;
  std::string header;
  std::map<std::string, std::string> fields;

  bool isReprValid;
  bool isDataType;

  Message() :
    isReprValid(false),
    isDataType(false)
  {}

public:
  typedef boost::shared_ptr<Message> Ptr;

  static Message::Ptr factory(std::string header, bool isData=false);

  void setField(std::string key, std::string value);
  void setFields(const std::map<std::string, std::string> &fields);
  const std::string getField(std::string key) const;
  const std::string& getHeader() const;

  virtual void setStream(std::istream* s_, int dataLength);
  virtual const std::string& toString();
  virtual void toSocket(boost::asio::ip::tcp::socket& socket);

  virtual ~Message() {}
};

class DataMessage : public Message {
  friend class Message;

  std::istream *stream_;
  int dataLength_;

protected:
  DataMessage()
    : stream_(NULL),
      dataLength_(0)
  { isDataType = true; }

public:
  typedef boost::shared_ptr<DataMessage> Ptr;

  void setStream(std::istream* s_, int dataLength);
  const std::string& toString();
  void toSocket(boost::asio::ip::tcp::socket& socket);

  ~DataMessage() { if (stream_ != NULL) delete stream_; }
};

typedef std::vector<Message::Ptr> MessagePtrContainer;

}

#endif


