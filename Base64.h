#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#include <string>

namespace FCPLib {

class Base64 {
  static char base64Alphabet[];
public:
  static std::string base64Encode(unsigned char *, int);
};

char Base64::base64Alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '~', '-'};

std::string
Base64::base64Encode(unsigned char *in, int len)
{
  std::string out(((len+2)/3)*4, '=');
  int o = 0;
  for (int i = 0; i < len;) {
    int val = (in[i++] & 0xFF) << 16;
    if (i < len)
      val |= (in[i++] & 0xFF) << 8;
    if (i < len)
      val |= (in[i++] & 0xFF);
    out[o++] = base64Alphabet[(val>>18) & 0x3F];
    out[o++] = base64Alphabet[(val>>12) & 0x3F];
    out[o++] = base64Alphabet[(val>>6) & 0x3F];
    out[o++] = base64Alphabet[val & 0x3F];
  }
  return out;
}

}

#endif // BASE64_H_INCLUDED
