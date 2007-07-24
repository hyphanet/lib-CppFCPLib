#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#include <string>
#include <stdexcept>

namespace FCPLib {


class IllegalBase64Exception : public std::logic_error {
public:
  IllegalBase64Exception(std::string str) : std::logic_error(str) {}
};

class Base64 {
  const static char base64Alphabet[];
  const static char base64Dealphabet[];
public:
  static std::string base64Encode(const unsigned char *, int);
  static std::string base64Decode(const std::string);
};

}

#endif // BASE64_H_INCLUDED
