#include "Message.h"


using namespace FCPLib;

Message::MessagePtr
Message::factory(std::string header){
  Message::MessagePtr m( new Message() );

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

std::string
Message::getField(const std::string &key) {
  return fields[key];
}


const std::string&
Message::toString() {
  static char intToString[30];
  if (isReprValid)
    return repr;
  repr = header + "\n";
  for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); ++it)
    if (isDataType && it->first == "Data")
      continue;
    else
      repr += it->first + "=" + it->second + "\n";
  if (isDataType) {
    repr += "DataLength=";
    sprintf(intToString, "%d", fields["Data"].size());     // FIXME
    repr += std::string(intToString);
    repr += "\n";
    repr += "Data\n";
    repr += fields["Data"];
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

