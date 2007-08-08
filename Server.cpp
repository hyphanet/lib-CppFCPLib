
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
    tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    socket_ = std::auto_ptr<boost::asio::ip::tcp::socket>( new boost::asio::ip::tcp::socket(io_service) );
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
  boost::system::error_code error;

  log().log(DEBUG, "Sending:\n"+s+"-----------------\n");
  if (!socket_->is_open())
    throw std::runtime_error("Server::send :: socket is closed");
  boost::asio::write(*socket_, boost::asio::buffer(s), boost::asio::transfer_all(), error );
  if (error)
    throw boost::system::system_error(error);
}

void Server::send(const Message::Ptr m)
{
  log().log(DEBUG, "Sending:\n"+m->toString()+"-----------------\n");

  m->toSocket(*socket_);
}

bool Server::dataAvailable(){
  boost::system::error_code error;
  bool ret;

  if (!socket_->is_open())
    throw std::runtime_error("Server::dataAvailable :: socket is closed");
  ret = socket_->available(error) != 0 || response.size() != 0;
  if (error)
    throw boost::system::system_error(error);

  return ret;
}

void
Server::read(char *buf, std::size_t len)
{
  log().log(NOISY, "Server::read: top");
  boost::system::error_code error;

  while (response.size() < len) {
    log().log(NOISY, "bytes read: " + boost::lexical_cast<std::string>(response.size()));
    response.commit(socket_->read_some(response.prepare(len), error));
    if (error)
      throw boost::system::system_error(error);
  }
  response_stream.read(buf, len);
}
