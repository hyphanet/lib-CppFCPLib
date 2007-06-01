#include "Message.h"


using namespace std;
using namespace FCPLib;

Message::Message() {
  isDataType = false;
  isReprValid = false;
}

Message::MessagePtr
Message::factory(std::string &header){
  Message::MessagePtr m( new Message() );

  m->header = header;

  return m;
}

Message::MessagePtr
Message::factory(const char *header){
  std::string hdr(header);

  return factory(hdr);
}

Message::MessagePtr
Message::factory(Server &s){
  Message::MessagePtr m( new Message() );
  static char line[1000];

  s.readln(line, 1000);
  line[strlen(line)-1] = 0;
  m->header = string(line);

  for (;;) {
    s.readln(line, 1000);
    line[strlen(line)-1] = 0;

    if (!strcmp(line, "End") || !strcmp(line, "EndMessage"))
      break;

    char *val = strchr(line, '=');
    *val++ = 0;
    m->fields[string(line)] = string(val);
  }

  return m;
}

void
Message::setField(std::string key, std::string value) {
  // TODO: should i check if a message can contain certain field?
  isReprValid = false;
  fields[key] = value;
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
  for (map<string, string>::iterator it = fields.begin(); it != fields.end(); ++it)
    if (isDataType && it->first == "Data")
      continue;
    else
      repr += it->first + "=" + it->second + "\n";
  if (isDataType) {
    repr += "DataLength=";
    sprintf(intToString, "%d", fields["Data"].size());
    repr += string(intToString);
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
