#ifndef FCPTESTDDARESPONSE_H_INCLUDED
#define FCPTESTDDARESPONSE_H_INCLUDED

#include <string>

namespace FCPLib {

class FCPTestDDAResponse {
public:
  FCPTestDDAResponse(std::string dir, bool read, bool write)
    : directory(dir),
      readDirectory(read),
      writeDirectory(write)
  {}
  std::string directory;
  bool readDirectory;
  bool writeDirectory;
};

}

#endif // FCPTESTDDARESPONSE_H_INCLUDED
