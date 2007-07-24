
#include "Base64.h"

using FCPLib::Base64;
using FCPLib::IllegalBase64Exception;

const char Base64::base64Alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '~', '-'};

const char Base64::base64Dealphabet[] = {
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62,
255, 255, 255,  63,  52,  53,  54,  55,  56,  57,  58,
 59,  60,  61, 255, 255, 255, 255, 255, 255, 255,   0,
  1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,
 12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,
 23,  24,  25, 255, 255, 255, 255, 255, 255,  26,  27,
 28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
 39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
 50,  51, 255, 255, 255, 255, 255 };


std::string
Base64::base64Encode(const unsigned char *in, int len)
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

std::string
Base64::base64Decode(const std::string inStr)
{
  const char *in = inStr.c_str();
  int inLength = inStr.size();

  // Strip trailing equals signs.
  while ((inLength > 0) && (in[inLength-1] == '='))
    inLength--;

  // check if all characters are valid
  for (int i = 0; i < inLength; ++i)
    if( !( isalpha(in[i]) || isdigit(in[i]) || in[i] == '~' || in[i] == '-' ) )
      throw IllegalBase64Exception("illegal Base64 character");

  int blocks = inLength/4;
  int remainder = inLength & 3;
  // wholeInLen and wholeOutLen are the the length of the input and output
  // sequences respectively, not including any partial block at the end.
  int wholeInLen  = blocks*4;
  int wholeOutLen = blocks*3;
  int outLen = wholeOutLen;
  switch (remainder) {
    case 1: throw IllegalBase64Exception("illegal Base64 length");
    case 2:  outLen = wholeOutLen+1; break;
    case 3:  outLen = wholeOutLen+2; break;
    default: outLen = wholeOutLen;
  }
  std::string out(outLen, ' ');
  int o = 0;
  int i;
  for (i = 0; i < wholeInLen;) {
    int in1 = base64Dealphabet[(int)in[i]];
    int in2 = base64Dealphabet[(int)in[i+1]];
    int in3 = base64Dealphabet[(int)in[i+2]];
    int in4 = base64Dealphabet[(int)in[i+3]];
    int orValue = in1|in2|in3|in4;
    if ((orValue & 0x80) != 0)
      throw IllegalBase64Exception("illegal Base64 character");
    int outVal = (in1 << 18) | (in2 << 12) | (in3 << 6) | in4;
    out[o] = (char) (outVal>>16);
    out[o+1] = (char) (outVal>>8);
    out[o+2] = (char) outVal;
    i += 4;
    o += 3;
  }
  int orValue = 0;
  switch (remainder) {
    case 2:
      {
        int in1 = base64Dealphabet[(int)in[i]];
        int in2 = base64Dealphabet[(int)in[i+1]];
        orValue = in1|in2;
        int outVal = (in1 << 18) | (in2 << 12);
        out[o] = (char) (outVal>>16);
      }
      break;
    case 3:
      {
        int in1 = base64Dealphabet[(int)in[i]];
        int in2 = base64Dealphabet[(int)in[i+1]];
        int in3 = base64Dealphabet[(int)in[i+2]];
        orValue = in1|in2|in3;
        int outVal = (in1 << 18) | (in2 << 12) | (in3 << 6);
        out[o] = (char) (outVal>>16);
        out[o+1] = (char) (outVal>>8);
      }
      break;
  }
  if ((orValue & 0x80) != 0)
    throw IllegalBase64Exception("illegal Base64 character");
  return out;
}
