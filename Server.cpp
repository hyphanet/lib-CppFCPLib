
#include "Server.h"
#include "Exceptions.h"
#include "DefaultValues.h"
#include <ostream>
#include <istream>
#include <boost/lexical_cast.hpp>

#include "Log.h"

using namespace FCPLib;


Server::Server(std::string &host, int port)
  : response_stream(&response){
    using boost::asio::ip::tcp;

    if ( host == "" ) host = "127.0.0.1";
    if ( port == -1 ) port = 9481;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, boost::lexical_cast<string>(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    socket_ = auto_ptr<boost::asio::ip::tcp::socket>( new boost::asio::ip::tcp::socket(io_service) );
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket_->close();
      socket_->connect(*endpoint_iterator++, error);
    }
    if (error)
      throw boost::system::system_error(error);
}

Server::~Server(){
}

std::string Server::readln(){
  boost::asio::read_until(*socket_, response, '\n');

  std::string line;
  std::getline(response_stream, line);

  return line;
}

void Server::send(const std::string &s){
  log().log(DEBUG, "Sending:\n"+s+"-----------------\n");
  boost::asio::streambuf request;
  std::ostream request_stream(&request);
  request_stream << s;
  boost::asio::write(*socket_, request);
}

void Server::send(const Message::MessagePtr m)
{
  log().log(DEBUG, "Sending:\n"+m->toString()+"-----------------\n");
  boost::asio::streambuf request;
  std::ostream request_stream(&request);
  m->toStream(request_stream);
  boost::asio::write(*socket_, request);
}

bool Server::dataAvailable(){
  return socket_->available() != 0 || response.size() != 0;
}
