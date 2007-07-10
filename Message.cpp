#include "Message.h"
#include <boost/lexical_cast.hpp>

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
  // TODO: should i check if a message can contain certain field?
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
    if (isDataType && it->first == "Data")
      continue;
    else
      repr += it->first + "=" + it->second + "\n";
  if (isDataType) {
    std::string& data = fields["Data"];
    repr += "DataLength=";
    repr += boost::lexical_cast<std::string>(data.size());
    repr += "\n";
    repr += "Data\n";
    repr += data;
  } else {
    repr += "EndMessage\n";
  }
  isReprValid = true;
  return repr;
}

const std::string&
Message::getHeader() const
{
  return header;
}

