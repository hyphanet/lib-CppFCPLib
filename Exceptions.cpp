#include "Exceptions.h"

using namespace FCPLib;

FCPException::FCPException(Message::Ptr m)
  : std::runtime_error(m->getHeader()),
    message(m)
{
}

FCPException::~FCPException() throw()
{
}


