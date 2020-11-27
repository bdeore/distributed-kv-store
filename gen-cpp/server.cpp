#include "dkvs.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

class dkvsHandler : virtual public dkvsIf {
 public:
  dkvsHandler() {
    // Your initialization goes here
    std::cout << "hello world" << std::endl;
  }

  void get(std::string &_return, const int16_t key, const std::string &consistency) override {
    // Your implementation goes here
    std::cout << "hello world" << std::endl;

  }

  void put(meta &_return, const int16_t key, const std::string &value, const std::string &consistency) override {
    // Your implementation goes here
    std::cout << "hello world" << std::endl;

  }

};

int main(int argc, char *argv[]) {
  int port = 9090;
  std::shared_ptr<dkvsHandler> handler(new dkvsHandler());
  std::shared_ptr<TProcessor> processor(new dkvsProcessor(handler));
  std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

