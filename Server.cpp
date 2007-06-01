
#include "Server.h"
#include "Exceptions.h"
#include "DefaultValues.h"
#include <cstring>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include "Log.h"

using namespace FCPLib;


Server::Server(std::string &host, int port){
	/* Use local host as default address */
	if (!host.size())
		host = "127.0.0.1";

	/* Use default port */
	if (port<=0)
	  port = 9481;

	struct hostent *he;
	struct sockaddr_in addr;

  /* Resolve hostname */
	if ((he = gethostbyname(host.c_str())) == NULL)
    throw StdError(__FUNCTION__, "Failed to resolve", strerror(errno));

	/* Snag socket */
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0))<0)
    throw StdError(__FUNCTION__, "Failed to get socket", strerror(errno));

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port);
	addr.sin_addr   = *((struct in_addr *)he->h_addr);
	memset(&(addr.sin_zero), '\0', 8);

	/* Connect */
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr))<0)	{
		close(sockfd);
		throw StdError(__FUNCTION__, "Failed to connect", strerror(errno));
	}
}

Server::~Server(){
  close(sockfd);
}

ssize_t Server::readn(void *vptr, size_t n){
  size_t nleft;
  ssize_t nread;
  char *ptr;

  ptr = (char*) vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nread = read(sockfd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else
        throw StdError(__FUNCTION__, "", strerror(errno));
    } else if (nread == 0) {
      break;   // EOF
    }
    nleft -= nread;
    ptr += nread;
  }

  return (n - nleft);
}

ssize_t Server::writen(const void *vptr, size_t n){
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = (const char*) vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nwritten = write(sockfd, ptr, nleft)) <= 0) {
      if (errno == EINTR)
        nwritten = 0;
      else
        throw StdError(__FUNCTION__, "", strerror(errno));
    }
    nleft -= nwritten;
    ptr += nwritten;
  }

  return (n);
}

ssize_t Server::readln(void *vptr, size_t maxlen){
  ssize_t n, rc;
  char c, *ptr;

  ptr = (char *)vptr;
  for (n = 1; n < maxlen; n++) {
    if ((rc = read(sockfd, &c, 1)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break;
    } else if (!rc) {
      if (n == 1)
        return 0;
      else
        break;
    }
  }
  *ptr = 0;
  return n;
}

void Server::send(const std::string &s){
  log().log(DEBUG, "Sending:\n"+s+"-----------------\n");
  writen(s.c_str(), s.length());
}

bool Server::dataAvailable(){
  int pollret;
  struct pollfd mypol;
	mypol.fd = sockfd;
  mypol.events = POLLERR | POLLHUP | POLLIN;

  pollret = poll(&mypol, 1, pollTimeout);

  if (pollret<0)
    throw new StdError(__FUNCTION__, "poll error", strerror(errno));

  if (!pollret)
    return false;

  /* if we get an error, die */
	if (mypol.revents&POLLERR)
	  throw new StdError(__FUNCTION__, "poll error", strerror(errno));

	if (mypol.revents&POLLHUP)
	  throw new StdError(__FUNCTION__, "poll hangup", strerror(errno));

  if (mypol.revents&POLLIN)
    return true;

  return false;
}
