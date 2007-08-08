
#ifndef SERVER_H__
#define SERVER_H__

#include <string>
#include <boost/asio.hpp>
#include <memory>
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
  void read(char*, std::size_t);
  void send(const std::string &s);
  void send(Message::Ptr m);
  bool dataAvailable();

  #ifdef _DEBUG_
  static std::auto_ptr<Server> create_server(std::string& host, int port)
  {
    return std::auto_ptr<Server>( new Server(host, port) );
  }
  #endif
};

}


#endif
