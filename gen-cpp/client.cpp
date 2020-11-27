#include "dkvs.h"
#include "store_types.h"
#include "store_constants.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <memory>

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

using std::make_shared;

int main(int argc, char *argv[]) {
  auto trans_ep = make_shared<TSocket>("128.226.114.205", 9092);
  auto trans_buf = make_shared<TBufferedTransport>(trans_ep);
  auto proto = make_shared<TBinaryProtocol>(trans_buf);
  dkvsClient client(proto);

  trans_ep->open();

  trans_ep->close();
}

