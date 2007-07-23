#include "Message.h"
#include "Server.h"
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
//#include <iostream>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(){
  std::string host = "127.0.0.1";
  int port = 10000;

  std::auto_ptr<FCPLib::Server> s = FCPLib::Server::create_server(host, port);
  /*
  FCPLib::Message::Ptr m = FCPLib::Message::factory( std::string("ClientHello") );
  m->setField("Name", "name_identifier");
  m->setField("ExpectedVersion", "2.0");

  s->send(m);
  s->send(m);
  s->send(m);
  */
  std::ifstream is("/home/mladen/Desktop/tech video/Navigating the World's Photographs.gvi");

  FCPLib::DataMessage::Ptr dm = boost::dynamic_pointer_cast<FCPLib::DataMessage,FCPLib::Message>
    ( FCPLib::Message::factory( std::string("DataMessage"), true ) );

  is.seekg(0, std::ios::end);
  int dataLength = is.tellg();
  is.seekg(0, std::ios::beg);
  dm->setStream(&is, dataLength);

  s->send(dm);

  getchar();
  return 0;
}
