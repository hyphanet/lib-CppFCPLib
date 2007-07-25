
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
  m->setField("PeerNoteType", peerNoteType);

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
  if (fields.hasField("Global")) m->setField("Global", fields.getField("Global"));
  if (fields.hasField("DontCompress")) m->setField("DontCompress", fields.getField("DontCompress"));
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence")) m->setField("Persistence", fields.getField("Persistence"));
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  m->setField("UploadFrom", "direct");

  m->setStream(s, dataLength);

  JobTicket::Ptr job = JobTicket::factory( m->getField("Identifier"), m );
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
  if (fields.hasField("mimetype")) m->setField("Metadata.ContentType", fields.getField("mimetype"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  // does not use chkonly
  if (fields.hasField("Global")) m->setField("Global", fields.getField("Global"));
  // does not use dontcompress
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence")) m->setField("Persistence", fields.getField("Persistence"));
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  m->setField("UploadFrom", "redirect");
  m->setField("TargetURI", target);

  JobTicket::Ptr job = JobTicket::factory( m->getField("Identifier"), m );
  log().log(DEBUG, job->toString());
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
  if (fields.hasField("mimetype")) m->setField("Metadata.ContentType", fields.getField("mimetype"));
  if (fields.hasField("Verbosity")) m->setField("Verbosity", fields.getField("Verbosity"));
  if (fields.hasField("MaxRetries")) m->setField("MaxRetries", fields.getField("MaxRetries"));
  if (fields.hasField("PriorityClass")) m->setField("PriorityClass", fields.getField("PriorityClass"));
  if (fields.hasField("GetCHKOnly")) m->setField("GetCHKOnly", fields.getField("GetCHKOnly"));
  if (fields.hasField("Global")) m->setField("Global", fields.getField("Global"));
  if (fields.hasField("DontCompress")) m->setField("DontCompress", fields.getField("DontCompress"));
  if (fields.hasField("ClientToken")) m->setField("ClientToken", fields.getField("ClientToken"));
  if (fields.hasField("Persistence")) m->setField("Persistence", fields.getField("Persistence"));
  if (fields.hasField("TargetFilename")) m->setField("TargetFilename", fields.getField("TargetFilename"));
  if (fields.hasField("EarlyEncode")) m->setField("EarlyEncode", fields.getField("EarlyEncode"));
  if (fields.hasField("BinaryBlob")) m->setField("BinaryBlob", fields.getField("BinaryBlob"));
  m->setField("UploadFrom", "disk");
  m->setField("Filename", filename);
  if (!r.readDirectory)
    m->setField("FileHash", fields.getField("FileHash"));

  JobTicket::Ptr job = JobTicket::factory( m->getField("Identifier"), m );
  log().log(DEBUG, job->toString());
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

    std::ifstream is(filename.c_str(), std::ios::binary);
    if (!is.is_open()) {
      log().log(ERROR, "Error while opening file :: " + filename);
      throw FileError("Error while opening file.", filename);
    }
    is.seekg(0, std::ios_base::end);
    int pos = is.tellg();
    is.seekg(0, std::ios_base::beg);

    return putData(URI, &is, pos, id, fields);
  }
}

JobTicket::Ptr
Node::subscribeUSK(const std::string USK, const std::string id, bool dontPoll)
{
  Message::Ptr m = Message::factory( std::string("SubscribeUSK") );
  m->setField("USK", USK);
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
