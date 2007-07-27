
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <typeinfo>
#include <fstream>
#include <sstream>
#include <cstring>

#include "Node.h"
#include "Log.h"
#include "Exceptions.h"
#include "Base64.h"
#include "Utils.h"

using namespace FCPLib;

std::string
Node::_getUniqueId() {
    char newid[100];
    sprintf(newid, "id%d", (int) time(0));
    return std::string(newid);
}

const Message::Ptr
Node::getNodeHelloMessage() const
{
  return nodeHelloMessage;
}

void
Node::checkProtocolError(Response &resp)
{
  ServerMessage::Ptr sm = createResult<ServerMessage::Ptr, LastMessageConverter>( resp );

  if ( sm->isError() )
    throw FCPException( sm->getMessage() );
}

Node::Node(std::string name_, std::string host, int port)
  : name(name_),
    clientReqQueue( new TQueue<JobTicket::Ptr>() ),
    globalCommandsTimeout(20)  // 20 sec
{
  if (!name.size())
    name = Node::_getUniqueId();
  log().log(DEBUG, "Node started name=" + name + "\n");

  try
  {
    nodeThread = new NodeThread(host, port, clientReqQueue);
  }
  catch (boost::system::system_error &e)
  {
    throw ConnectionRefused(e.what());
  }
  executor.execute( nodeThread );

  Message::Ptr m = Message::factory(std::string("ClientHello"));
  m->setField("Name", name);
  m->setField("ExpectedVersion", "2.0");

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "Node constructor: waiting for response to ClientHello");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // check if CloceConnectionDuplicateName or ProtocolError has arrived
  checkProtocolError(resp); // throws

  nodeHelloMessage = createResult<Message::Ptr, MessageConverter>( resp );
}

Node::~Node()
{
  executor.interrupt();
}

Message::Ptr
Node::listPeer(const std::string & identifier)
{
  Message::Ptr m = Message::factory( std::string("ListPeer") );

  m->setField("NodeIdentifier", identifier);

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for Peer message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

MessagePtrContainer
Node::listPeers(const AdditionalFields& fields)
{
  Message::Ptr m = Message::factory( std::string("ListPeers") );
  if (fields.hasField("WithMetadata")) m->setField("WithMetadata", fields.getField("WithMetadata"));
  if (fields.hasField("WithVolatile")) m->setField("WithVolatile", fields.getField("WithVolatile"));

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for EndListPeers message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // NOTE: error should never happen here...
  checkProtocolError(resp); // throws

  return createResult<MessagePtrContainer, VectorWithoutLastConverter>( resp );
}

PeerNoteContainer
Node::listPeerNotes(const std::string& identifier)
{
  Message::Ptr m = Message::factory( std::string("ListPeerNotes") );
  m->setField("NodeIdentifier", identifier);

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for EndListPeerNotes message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<PeerNoteContainer, PeerNotesConverter>( resp );
}

Message::Ptr
Node::addPeer(const std::string &value, bool isURL = false) {
  Message::Ptr m = Message::factory( std::string("AddPeer") );
  if (!isURL)
    m->setField("File", value);
  else
    m->setField("URL", value);

  JobTicket::Ptr job = JobTicket::factory( "", m);
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for Peer message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

Message::Ptr
Node::addPeer(const std::map<std::string, std::string> &message)
{
  Message::Ptr m = Message::factory( std::string("AddPeer") );

  m->setFields(message);

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for Peer message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

Message::Ptr
Node::modifyPeer(const std::string & nodeIdentifier,
                 const AdditionalFields& fields)
{
  Message::Ptr m = Message::factory( std::string("ModifyPeer") );

  m->setField("NodeIdentifier", nodeIdentifier);
  if (fields.hasField("AllowLocalAddresses")) m->setField("AllowLocalAddresses", fields.getField("AllowLocalAddresses"));
  if (fields.hasField("IsDisabled")) m->setField("IsDisabled", fields.getField("IsDisabled"));
  if (fields.hasField("IsListenOnly")) m->setField("IsListenOnly", fields.getField("IsListenOnly"));

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for Peer message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

PeerNote
Node::modifyPeerNote(const std::string & nodeIdentifier,
                     const std::string & noteText,
                     int peerNoteType)
{
  Message::Ptr m = Message::factory( std::string("ModifyPeerNote") );

  m->setField("NodeIdentifier", nodeIdentifier);
  m->setField("NoteText", Base64::base64Encode((const unsigned char*)noteText.c_str(), noteText.size()));
  m->setField("PeerNoteType", boost::lexical_cast<string>( peerNoteType ));

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for PeerNote message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<PeerNote, PeerNoteConverter>( resp );
}

Message::Ptr
Node::removePeer(const std::string &identifier)
{
  Message::Ptr m = Message::factory( std::string("RemovePeer") );

  m->setField("NodeIdentifier", identifier);

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for PeerRemoved message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

Message::Ptr
Node::getNode(const AdditionalFields& fields)
{
  Message::Ptr m = Message::factory( std::string("GetNode") );

  if (fields.hasField("GiveOpennetRef")) m->setField("GiveOpennetRef", fields.getField("GiveOpennetRef"));
  if (fields.hasField("WithPrivate")) m->setField("WithPrivate", fields.getField("WithPrivate"));
  if (fields.hasField("WithVolatile")) m->setField("WithVolatile", fields.getField("WithVolatile"));

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for NodeData message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

Message::Ptr
Node::getConfig(const AdditionalFields& fields)
{
  Message::Ptr m = Message::factory( std::string("GetConfig") );

  if (fields.hasField("WithCurrent")) m->setField("WithCurrent", fields.getField("WithCurrent"));
  if (fields.hasField("WithDefault")) m->setField("WithDefault", fields.getField("WithDefault"));
  if (fields.hasField("WithSortOrder")) m->setField("WithSortOrder", fields.getField("WithSortOrder"));
  if (fields.hasField("WithExpertFlag")) m->setField("WithExpertFlag", fields.getField("WithExpertFlag"));
  if (fields.hasField("WithForceWriteFlag")) m->setField("WithForceWriteFlag", fields.getField("WithForceWriteFlag"));
  if (fields.hasField("WithShortDescription")) m->setField("WithShortDescription", fields.getField("WithShortDescription"));
  if (fields.hasField("WithLongDescription")) m->setField("WithLongDescription", fields.getField("WithLongDescription"));

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for ConfigData message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

Message::Ptr
Node::modifyConfig(Message::Ptr m)
{
  if (m->getHeader() != "ModifyConfig")
    throw std::logic_error("ModifyConfig message expected, " + m->getHeader() + " received");

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for ConfigData message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // ProtocolError or UnknownNodeIdentifier
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

TestDDAReplyResponse::Ptr
Node::testDDARequest(std::string dir, bool read, bool write)
{
  Message::Ptr m = Message::factory( std::string("TestDDARequest") );

  m->setField("Directory", dir);
  if (read)
    m->setField("WantReadDirectory", "true");
  if (write)
    m->setField("WantWriteDirectory", "true");

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for TestDDAReply");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // check if protocol error has occured
  checkProtocolError(resp); // throws

  return createResult<TestDDAReplyResponse::Ptr, TestDDAReplyConverter>( resp );
}

TestDDAResponse
Node::testDDAResponse(std::string dir, std::string readContent)
{
  Message::Ptr m = Message::factory( std::string("TestDDAResponse") );

  m->setField("Directory", dir);
  if (readContent != "")
    m->setField("ReadContent", readContent);

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for TestDDAComplete");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  // check if protocol error has occured
  checkProtocolError(resp); // throws

  m = createResult<Message::Ptr, MessageConverter>( resp );
  return TestDDAResponse(m->getField("Directory"),
                            m->getField("ReadDirectoryAllowed")=="true",
                            m->getField("WriteDirectoryAllowed")=="true");
}

TestDDAResponse
Node::testDDA(std::string dir, bool read, bool write)
{
   TestDDAReplyResponse::Ptr replyResponse;
   Message::Ptr m;
   boost::filesystem::path filePath;

   try
   {
     replyResponse = testDDARequest(dir, read, write);

     std::ostringstream readContent;
     if (read) {
       std::ifstream is(replyResponse->getReadFilename().c_str());
       // check that file is opened
       if (is) {
         readContent << is.rdbuf();
       }
     }
     if (write) {
       filePath = boost::filesystem::path( replyResponse->getWriteFilename() );
       std::ofstream os(replyResponse->getWriteFilename().c_str());
       if (os) {
         os << replyResponse->getContent();;
         os.close();
       }
     }
     TestDDAResponse ret = testDDAResponse(dir, readContent.str());
     try {
       if (write) boost::filesystem::remove( filePath );
     } catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path>& e) {
       log().log(ERROR, e.what());
     }
     return ret;
   }
   catch (FCPException& e)
   {
     log().log(ERROR, e.getMessage()->toString());
   }
   catch (std::logic_error& e)
   {
     log().log(FATAL, e.what()); // this should never happen... TODO: should I force shutdown?
   }
   catch (std::runtime_error& e)
   {
     log().log(ERROR, e.what());
   }
   catch (std::exception& e)
   {
     log().log(ERROR, e.what());
   }
   // can happen if testDDAResponse throws
   try {
     if (write) boost::filesystem::remove( filePath );
   } catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path>& e) {
     log().log(ERROR, e.what());
   }
   return TestDDAResponse(dir, false, false);
}

Message::Ptr
Node::generateSSK(std::string identifier)
{
  Message::Ptr m = Message::factory( std::string("GenerateSSK") );
  m->setField("Identifier", identifier);

  JobTicket::Ptr job = JobTicket::factory( identifier, m );
  clientReqQueue->put(job);

  log().log(DEBUG, "waiting for SSKKeypair message");
  job->wait(globalCommandsTimeout);

  Response resp = job->getResponse();
  checkProtocolError(resp); // throws

  return createResult<Message::Ptr, MessageConverter>( resp );
}

JobTicket::Ptr
Node::putData(const std::string URI, std::istream* s, int dataLength, const std::string id, const AdditionalFields& fields )
{
  Message::Ptr m = Message::factory( std::string("ClientPut"), true );

  m->setField("URI", URI);
  m->setField("Identifier", id == "" ? _getUniqueId() : id);
  if (fields.hasField("Metadata.ContentType")) m->setField("Metadata.ContentType", fields.getField("Metadata.ContentType"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  if (fields.hasField("GetCHKOnly")) m->setField("GetCHKOnly", fields.getField("GetCHKOnly"));
  if (fields.hasField("Global"))
    m->setField("Global", fields.getField("Global"));
  else
    m->setField("Global", "false");
  if (fields.hasField("DontCompress")) m->setField("DontCompress", fields.getField("DontCompress"));
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence"))
    m->setField("Persistence", fields.getField("Persistence"));
  else
    m->setField("Persistence", "connection");
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  m->setField("UploadFrom", "direct");
  m->setStream(s, dataLength);

  bool persistent = m->getField("Persistence") != "connection";
  bool global = boost::lexical_cast<bool>( m->getField("Global") );
  if (global && !persistent)
    throw std::invalid_argument("Global requests must be persistent");

  JobTicket::Ptr job = JobTicket::factory( m->getField("Identifier"), m );
  job->setGlobal( global ).setPersistent( persistent );
  clientReqQueue->put(job);

  job->waitTillReqSent(globalCommandsTimeout); // assure that there is a response

  return job;
}

JobTicket::Ptr
Node::putRedirect(const std::string URI, const std::string target, const std::string id, const AdditionalFields& fields )
{
  Message::Ptr m = Message::factory( std::string("ClientPut"));

  m->setField("URI", URI);
  m->setField("Identifier", id == "" ? _getUniqueId() : id);
  if (fields.hasField("Metadata.ContentType")) m->setField("Metadata.ContentType", fields.getField("Metadata.ContentType"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  // does not use chkonly
  if (fields.hasField("Global"))
    m->setField("Global", fields.getField("Global"));
  else
    m->setField("Global", "false");
  // does not use dontcompress
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence"))
    m->setField("Persistence", fields.getField("Persistence"));
  else
    m->setField("Persistence", "connection");
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  m->setField("UploadFrom", "redirect");
  m->setField("TargetURI", target);

  bool persistent = m->getField("Persistence") != "connection";
  bool global = boost::lexical_cast<bool>( m->getField("Global") );
  if (global && !persistent)
    throw std::invalid_argument("Global requests must be persistent");

  JobTicket::Ptr job = JobTicket::factory( m->getField("Identifier"), m );
  job->setGlobal( global ).setPersistent( persistent );
  clientReqQueue->put(job);

  job->waitTillReqSent(globalCommandsTimeout); // assure that there is a response

  return job;
}

JobTicket::Ptr
Node::putDisk(const std::string URI, const std::string filename, const std::string id, const AdditionalFields& fields )
{
  std::string identifier = id == "" ? _getUniqueId() : id;

  // try with TestDDA
  // we want to read somethig from a filesystem

  // extract dir
  boost::filesystem::path filePath( filename );
  std::string dir = filePath.branch_path().string();
  TestDDAResponse r = this->testDDA(dir, true, false); // read only

  std::string filehash ( fields.hasField("FileHash") ? fields.getField("FileHash") : "" );

  if (!r.readDirectory && filehash == "") { // try hash
    std::ifstream is(filename.c_str(), std::ios::binary);
    if (!is.is_open()) {
      log().log(ERROR, "Error while opening file :: " + filename);
      throw FileError("Error while opening file.", filename);
    }
    unsigned char buf[1024];

    std::string salt = nodeHelloMessage->getField("ConnectionIdentifier") + "-" + identifier;
    int salt_size = salt.size();
    unsigned char *salt_ptr = new unsigned char[salt_size];
    memcpy(salt_ptr, salt.c_str(), salt_size);

    SHA256 sha;
    sha.write(salt_ptr, salt_size);
    delete [] salt_ptr;
    while (true) {
      int bytes_read;
      is.read((char*)buf, 1024);
      if (is.fail()) {
        log().log(ERROR, "Error while reading file :: " + filename);
        throw FileError("Error while reading file.", filename);
      }
      bytes_read = is.gcount();
      if (!bytes_read) break;
      sha.write(buf, bytes_read);
    }
    sha.final();
    filehash =  Base64::base64Encode(sha.read(), 32);
  }

  Message::Ptr m = Message::factory( std::string("ClientPut") );

  m->setField("URI", URI);
  m->setField("Identifier", identifier);
  if (fields.hasField("Metadata.ContentType")) m->setField("Metadata.ContentType", fields.getField("Metadata.ContentType"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  if (fields.hasField("GetCHKOnly")) m->setField("GetCHKOnly", fields.getField("GetCHKOnly"));
  if (fields.hasField("Global"))
    m->setField("Global", fields.getField("Global"));
  else
    m->setField("Global", "false");
  if (fields.hasField("DontCompress")) m->setField("DontCompress", fields.getField("DontCompress"));
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence"))
    m->setField("Persistence", fields.getField("Persistence"));
  else
    m->setField("Persistence", "connection");
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  m->setField("UploadFrom", "disk");
  m->setField("Filename", filename);
  if (!r.readDirectory)
    m->setField("FileHash", fields.getField("FileHash"));

  bool persistent = m->getField("Persistence") != "connection";
  bool global = boost::lexical_cast<bool>( m->getField("Global") );
  if (global && !persistent)
    throw std::invalid_argument("Global requests must be persistent");

  JobTicket::Ptr job = JobTicket::factory( m->getField("Identifier"), m );
  job->setGlobal( global ).setPersistent( persistent );
  clientReqQueue->put(job);

  job->waitTillReqSent(globalCommandsTimeout); // assure that there is a response

  try
  {
    Response resp = job->getResponse();
    checkProtocolError(resp); // throws
    return job;
  }
  catch (FCPException &e)
  {
    // error, try direct mode

    std::ifstream* is = new std::ifstream(filename.c_str(), std::ios::binary);
    if (!is->is_open()) {
      delete is;
      log().log(ERROR, "Error while opening file :: " + filename);
      throw FileError("Error while opening file.", filename);
    }
    is->seekg(0, std::ios_base::end);
    int pos = is->tellg();
    is->seekg(0, std::ios_base::beg);

    return putData(URI, is, pos, id, fields);
  }
}

GetJob::Ptr
Node::getDisk(const std::string URI, const std::string filename, const std::string id, const AdditionalFields& fields )
{
  std::string identifier = id == "" ? _getUniqueId() : id;
  bool persistent = fields.hasField("Persistence") && fields.getField("Persistence") != "connection";
  bool global = fields.hasField("Global") && fields.getField("Global") == "true";
  if (global && !persistent)
    throw std::invalid_argument("Global requests must be persistent");

  // TestDDA, we want to write somethig from a filesystem
  // extract dir
  boost::filesystem::path filePath( filename );
  std::string dir = filePath.branch_path().string();
  TestDDAResponse r = this->testDDA(dir, false, true); // read only

  if ( r.writeDirectory ) {
    Message::Ptr m = Message::factory( std::string("ClientGet") );

    m->setField("URI", URI);
    m->setField("Identifier", identifier);
    if (fields.hasField("IgnoreDS")) m->setField("IgnoreDS", fields.getField("IgnoreDS"));
    if (fields.hasField("DSonly")) m->setField("DSonly", fields.getField("DSonly"));
    if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
    if (fields.hasField("MaxSize")) m->setField("MaxSize", fields.getField("MaxSize"));
    if (fields.hasField("MaxTempSize")) m->setField("MaxTempSize", fields.getField("MaxTempSize"));
    if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
    if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
    if (fields.hasField("Persistence"))
      m->setField("Persistence", fields.getField("Persistence"));
    else
      m->setField("Persistence", "connection");
    if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
    m->setField("Global", Converter::toString(global));
    m->setField("ReturnType", "disk");
    if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
    if (fields.hasField("AllowedMIMETypes")) m->setField("AllowedMIMETypes", fields.getField("AllowedMIMETypes"));
    m->setField("Filename", filename);
    if (fields.hasField("TempFilename")) m->setField("TempFilename", fields.getField("TempFilename"));

    GetJob::Ptr job = GetJob::factory( m->getField("Identifier"), m );
    job->setGlobal( global ).setPersistent( persistent );
    job->setReturnType( GetJob::Disk );

    clientReqQueue->put(job);

    return job;
  }

  // allow fallback to direct only to non persistent requests
  // otherwise you can start a direct get with intention to store the data to disk
  // but when client loses a connection it loses information where to store data as well
  if ( !r.writeDirectory && persistent )
    throw TestDDAError("TestDDA write failed");

  // TODO :: implement fallback
}

GetJob::Ptr
Node::fetchData(const std::string URI, const std::string id, const AdditionalFields& fields)
{
  std::string identifier = id == "" ? _getUniqueId() : id;
  bool persistent = fields.hasField("Persistence") && fields.getField("Persistence") != "connection";
  bool global = fields.hasField("Global") && fields.getField("Global") == "true";
  if (global && !persistent)
    throw std::invalid_argument("Global requests must be persistent");

  Message::Ptr m = Message::factory( std::string("ClientGet") );

  m->setField("URI", URI);
  m->setField("Identifier", identifier);
  if (fields.hasField("IgnoreDS")) m->setField("IgnoreDS", fields.getField("IgnoreDS"));
  if (fields.hasField("DSonly")) m->setField("DSonly", fields.getField("DSonly"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxSize")) m->setField("MaxSize", fields.getField("MaxSize"));
  if (fields.hasField("MaxTempSize")) m->setField("MaxTempSize", fields.getField("MaxTempSize"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  if (fields.hasField("Persistence"))
    m->setField("Persistence", fields.getField("Persistence"));
  else
    m->setField("Persistence", "connection");
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  m->setField("Global", Converter::toString(global));
  m->setField("ReturnType", "none");
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  if (fields.hasField("AllowedMIMETypes")) m->setField("AllowedMIMETypes", fields.getField("AllowedMIMETypes"));

  GetJob::Ptr job = GetJob::factory( m->getField("Identifier"), m );
  job->setGlobal( global ).setPersistent( persistent );
  job->setReturnType( GetJob::None );

  clientReqQueue->put(job);

  return job;
}
GetJob::Ptr
Node::getDirect(const std::string URI, const std::string id, std::ostream* stream, const AdditionalFields& fields)
{
  // TODO:: implement for persistent
  std::string identifier = id == "" ? _getUniqueId() : id;
  bool persistent = fields.hasField("Persistence") && fields.getField("Persistence") != "connection";
  bool global = fields.hasField("Global") && fields.getField("Global") == "true";
  if (global && !persistent)
    throw std::invalid_argument("Global requests must be persistent");

  if (stream == NULL && !persistent)
    throw std::invalid_argument("You must specify stream when strarting Get with Persistence=connection");

  Message::Ptr m = Message::factory( std::string("ClientGet") );

  m->setField("URI", URI);
  m->setField("Identifier", identifier);
  if (fields.hasField("IgnoreDS")) m->setField("IgnoreDS", fields.getField("IgnoreDS"));
  if (fields.hasField("DSonly")) m->setField("DSonly", fields.getField("DSonly"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxSize")) m->setField("MaxSize", fields.getField("MaxSize"));
  if (fields.hasField("MaxTempSize")) m->setField("MaxTempSize", fields.getField("MaxTempSize"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  if (fields.hasField("Persistence"))
    m->setField("Persistence", fields.getField("Persistence"));
  else
    m->setField("Persistence", "connection");
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  m->setField("Global", Converter::toString(global));
  m->setField("ReturnType", "direct");
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  if (fields.hasField("AllowedMIMETypes")) m->setField("AllowedMIMETypes", fields.getField("AllowedMIMETypes"));

  GetJob::Ptr job = GetJob::factory( m->getField("Identifier"), m );
  job->setGlobal( global ).setPersistent( persistent );
  job->setReturnType( GetJob::Direct ).setStream( stream );

  clientReqQueue->put(job);

  return job;
}


JobTicket::Ptr
Node::subscribeUSK(const std::string URI, const std::string id, bool dontPoll)
{
  Message::Ptr m = Message::factory( std::string("SubscribeUSK") );
  m->setField("URI", URI);
  m->setField("Identifier", id);
  m->setField("DontPoll", Converter::toString( dontPoll ));

  JobTicket::Ptr job = JobTicket::factory( id, m );
  clientReqQueue->put(job);

  return job;
}

void
Node::watchGlobal( bool enabled, int verbosity )
{
  Message::Ptr m = Message::factory( std::string("WatchGlobal") );
  m->setField( "Enabled", Converter::toString( enabled ) );
  m->setField( "VerbosityMask", boost::lexical_cast<std::string>(verbosity) );

  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);
}

void
Node::refreshPersistentRequest()
{
  Message::Ptr m = Message::factory( std::string("ListPersistentRequest") );
  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  // persistent jobs will be updated

  log().log(DEBUG, "waiting for EndListPersistentRequests message");
  job->wait(globalCommandsTimeout);
}

JobCollection
Node::listGlobalJobs()
{
  JobCollection ret;
  std::map<std::string, JobTicket::Ptr >::iterator it;

  for (it = nodeThread->jobs[1].begin(); it != nodeThread->jobs[1].end(); ++it)
    ret.push_back(it->second);

  return ret;
}

JobCollection
Node::listPersistentJobs()
{
  JobCollection ret;
  std::map<std::string, JobTicket::Ptr >::iterator it;

  for (int i = 0; i < 2; ++i)
    for (it = nodeThread->jobs[i].begin(); it != nodeThread->jobs[i].end(); ++it)
      if (it->second->isPersistent()) ret.push_back(it->second);

  return ret;
}

void
Node::removePersistentRequest( JobTicket::Ptr job )
{
  if ( !job->isPersistent() )
    throw std::invalid_argument("Job is not persistent");

  Message::Ptr m = Message::factory( std::string("PersistentRequestRemoved ") );
  m->setField( "Global", Converter::toString( job->isGlobal() ) );
  m->setField( "Identifier", job->getId() );

  JobTicket::Ptr job_ = JobTicket::factory( "", m );
  clientReqQueue->put(job_);

  // wait on job that is to be cancelled
  job->wait(globalCommandsTimeout);
}

void
Node::modifyPersistentRequest( JobTicket::Ptr job, const AdditionalFields& fields )
{
  if ( !job->isPersistent() )
    throw std::invalid_argument("Job is not persistent");

  bool changed = false;

  Message::Ptr m = Message::factory( std::string("ModifyPersistentRequest ") );
  m->setField( "Identifier", job->getId() );
  m->setField( "Global", Converter::toString( job->isGlobal() ) );
  if (fields.hasField("PriorityClass")) {
    m->setField("PriorityClass", fields.getField("PriorityClass"));
    changed = true;
  }
  if (fields.hasField("ClientToken")) {
    m->setField("ClientToken", fields.getField("ClientToken"));
    changed = true;
  }
  if (!changed)
    throw std::invalid_argument("Either ClientToken or PriorityClass needs to be defined.");

  JobTicket::Ptr job_ = JobTicket::factory( "", m );
  clientReqQueue->put(job_);
}

void
Node::shutdown()
{
  log().log(DEBUG, "about to shutdown the node");
  Message::Ptr m = Message::factory( std::string("Shutdown") );
  JobTicket::Ptr job = JobTicket::factory( "", m );
  clientReqQueue->put(job);

  try {
    job->wait(globalCommandsTimeout);
    Response resp = job->getResponse();

    try {
      checkProtocolError(resp);
    } catch ( FCPException& e) {
      if ( boost::lexical_cast<int>( e.getMessage()->getField("Code") ) != 18 )
        throw e;
      executor.interrupt();
      log().log(DEBUG, "node is shutdown");
    }
  } catch (...) {
    log().log(ERROR, "error has occured while node shutdown, kill thread anyway");
    executor.interrupt();
    log().log(DEBUG, "node thread killed");
  }
}
