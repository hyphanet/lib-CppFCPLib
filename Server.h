
#ifndef SERVER_H__
#define SERVER_H__

#include <string>

namespace FCPLib {

class Server {
  friend class NodeThread;
  int sockfd;
  Server(std::string &host, int port=-1);
public:
  ~Server();
  ssize_t readn(void *vptr, size_t n);
  ssize_t readln(void *vptr, size_t maxlen);
  ssize_t writen(const void *vptr, size_t n);
  void send(const std::string &s);
  bool dataAvailable();
};

}


#endif
