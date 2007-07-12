
#ifndef SERVER_H__
#define SERVER_H__

#include <string>
#include <boost/asio.hpp>
#include "Message.h"

namespace FCPLib {

class Server {
  friend class NodeThread;

  boost::asio::io_service io_service;
  std::auto_ptr<boost::asio::ip::tcp::socket> socket_;
  boost::asio::streambuf response;
  std::istream response_stream;

  Server(std::string& host, int port);
public:
  ~Server();
  std::string readln();
  void send(const std::string &s);
  void send(Message::MessagePtr m);
  bool dataAvailable();
};

}


#endif
