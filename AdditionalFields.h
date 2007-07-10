#ifndef ADDITIONALFIELDS_H_INCLUDED
#define ADDITIONALFIELDS_H_INCLUDED

#include <map>
#include <string>
#include <boost/lexical_cast.hpp>
#include <exception>

namespace FCPLib {

class AdditionalFields {
  std::map<std::string, std::string> fields;
public:
  AdditionalFields() {}
  void addField(std::string key, int value) {
    fields[key] = boost::lexical_cast<std::string>(value);
  }
  void addField(std::string key, std::string value) {
    fields[key] = value;
  }
  void addField(std::string key, bool value) {
    fields[key] = boost::lexical_cast<std::string>(value);
  }
  bool hasField(std::string key) const {
    if (fields.find(key) == fields.end()) return false;
    return true;
  }
  const std::string& getField(std::string key) const throw(std::invalid_argument){
    std::map<std::string, std::string>::const_iterator it;
    it = fields.find(key);
    if (it == fields.end())
      throw new std::invalid_argument("Additional fields object does not contain: " + key);
    return it->second;
  }
};

}

#endif // ADDITIONALFIELDS_H_INCLUDED
