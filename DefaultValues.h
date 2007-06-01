
#ifndef DEFAULTVALUES_H_
#define DEFAULTVALUES_H_

#include <string>
//#include "Log.h"

const std::string defaultFCPHost = "127.0.0.1";
const int defaultFCPPort = 9481;
const std::string defaultFProxyHost = "127.0.0.1";
const int defaultFProxyPort = 8888;
//const FCPLib::verbosityLevel defaultVerbosity = FCPLib::ERROR;
const int inputBufferSize = 64 * 1024;
const int outputBufferSize = 64 * 1024;
const int maxSizeMessage = 8192;
const int pollTimeout = 100;
const int oneyear = 86400 * 365 * 1000;

#endif
