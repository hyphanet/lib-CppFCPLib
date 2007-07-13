#include "Message.h"
#include <boost/lexical_cast.hpp>
#include <iterator>
#include <algorithm>

using namespace FCPLib;

Message::MessagePtr
Message::factory(std::string header, bool isData){
  Message::MessagePtr m;
  if (!isData)
    m = Message::MessagePtr( new Message() );
  else
    m = Message::MessagePtr( new DataMessage() );

  m->header = header;

  return m;
}

void
Message::setField(std::string key, std::string value) {
  isReprValid = false;
  fields[key] = value;
}

void
Message::setFields(const std::map<std::string, std::string> &fields) {
  for (std::map<std::string, std::string>::const_iterator it = fields.begin(); it != fields.end(); ++it) {
    this->setField(it->first, it->second);
  }
}

const std::string
Message::getField(std::string key) const
{
  std::map<std::string, std::string>::const_iterator it;
  it = fields.find(key);
  if (it == fields.end())
    return "";
  else
    return it->second;
}

const std::string&
Message::toString() {
  if (isReprValid)
    return repr;
  repr = header + "\n";
  for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); ++it)
      repr += it->first + "=" + it->second + "\n";
  if (!isDataType)
    repr += "EndMessage\n";
  isReprValid = true;
  return repr;
}

const std::string&
Message::getHeader() const
{
  return header;
}

void
Message::toStream(std::ostream& output_stream)
{
  output_stream << toString();
  output_stream.flush();
}

void
Message::toSocket(boost::asio::ip::tcp::socket& socket)
{
  boost::asio::write(socket, boost::asio::buffer(toString()));
}

void
DataMessage::setStream(std::istream* s_, int dataLength)
{
  stream_ = s_;
  dataLength_ = dataLength;
}

const std::string&
DataMessage::toString() {
  if (isReprValid)
    return repr;
  Message::toString();
  repr += "DataLength=" + boost::lexical_cast<std::string>(dataLength_);
  repr += "Data\n";
  isReprValid = true;
  return repr;
}

void
DataMessage::toStream(std::ostream& output_stream)
{
  output_stream << toString();
  std::copy(std::istream_iterator<char> (*stream_),
            std::istream_iterator<char> (),
            std::ostream_iterator<char> (output_stream));
  output_stream.flush();
}

void
DataMessage::toSocket(boost::asio::ip::tcp::socket& socket)
{
  boost::asio::write( socket, boost::asio::buffer(toString()) );
  boost::asio::write( socket, boost::asio::buffer(stream_->rdbuf(), dataLength_)  );
}

