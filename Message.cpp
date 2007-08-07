
#include <boost/lexical_cast.hpp>
#include <iterator>
#include <algorithm>

#include "Message.h"
#include "Exceptions.h"

using namespace FCPLib;

Message::Ptr
Message::factory(std::string header, bool isData){
  Message::Ptr m;
  if (!isData)
    m = Message::Ptr( new Message() );
  else
    m = Message::Ptr( new DataMessage() );

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

void
Message::setStream(std::istream* s_, int dataLength)
{
  throw NotImplemented("Message::setStream is not implemented.");
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
Message::toSocket(boost::asio::ip::tcp::socket& socket)
{
  boost::system::error_code error;
  if (!socket.is_open())
    throw std::runtime_error("Message::toSocket :: socket is closed");
  boost::asio::write(socket, boost::asio::buffer(toString()), boost::asio::transfer_all(), error);
  if (error)
    throw boost::system::system_error(error);
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
  repr += "DataLength=" + boost::lexical_cast<std::string>(dataLength_) + "\n";
  repr += "Data\n";
  isReprValid = true;
  return repr;
}


void
DataMessage::toSocket(boost::asio::ip::tcp::socket& socket)
{
  boost::system::error_code error;
  char buf[1024];
  int tmp = dataLength_;

  if (!socket.is_open())
    throw std::runtime_error("DataMessage::toSocket :: socket is closed");
  boost::asio::write( socket, boost::asio::buffer(toString()), boost::asio::transfer_all(), error );
  if (error)
    throw boost::system::system_error(error);

  while (tmp > 0) {
    int m = std::min<int>(tmp, 1024);
    stream_->read(buf, m);
    if (stream_->fail())
      throw std::runtime_error("Error while reading data stream.");
    if (!socket.is_open())
      throw std::runtime_error("DataMessage::toSocket :: socket is closed");
    boost::asio::write( socket, boost::asio::buffer(buf, m), boost::asio::transfer_all(), error );
    if (error)
      throw boost::system::system_error(error);
    tmp -= m;
  }
}

