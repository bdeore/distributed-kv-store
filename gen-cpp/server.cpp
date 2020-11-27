#include "dkvs.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <utility>
#include <vector>
#include <chrono>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

int get_time_in_seconds();

class replica_node {
 public:
  std::string ip;
  int port, range_begin, range_end;

  replica_node(std::string ip, int port, int range_begin, int range_end)
      : ip(std::move(ip)), port(port), range_begin(range_begin), range_end(range_end) {}
};

class dkvsHandler : virtual public dkvsIf {
 private:
  std::fstream snitch;
  std::stringstream node_info;
  std::string line;
  int range_begin, range_end;
  std::vector<replica_node> nodes;
  std::map<int16_t, std::string> mem_table;

 public:
  int port;
  std::string ip;

  explicit dkvsHandler(std::string &snitch_file) {
    int node_port, node_range_begin, node_range_end;
    std::string node_ip;

    snitch.open(snitch_file);
    if (snitch.is_open()) {
      getline(snitch, line);
      node_info.str(line);
      node_info >> node_ip >> node_port >> node_range_begin >> node_range_end;

      this->ip = node_ip;
      this->port = node_port;
      this->range_begin = node_range_begin;
      this->range_end = node_range_end;

      while (getline(snitch, line)) {
        std::stringstream temp;
        temp.str(line);
        temp >> node_ip >> node_port >> node_range_begin >> node_range_end;
        replica_node node(node_ip, node_port, node_range_begin, node_range_end);
        nodes.push_back(node);
      }
    }
  }

  void get(meta &_return, const int16_t key, const std::string &consistency) override {
    std::string value, time_stamp;
    std::stringstream ss;

    auto it = mem_table.find(key);
    if (it == mem_table.end()) {
      _return.__set_result("");
      _return.__set_success(false);
    } else {
      ss << it->second;
      ss >> value >> time_stamp;

      std::cout << "value: " << value << " timestamp: " << time_stamp << std::endl;
      _return.__set_result(value);
      _return.__set_success(true);
      _return.__set_timestamp(std::stoi(time_stamp));
    }
    _return.__set_ip(ip);
    _return.__set_port(port);
  }

  void put(meta &_return, const int16_t key, const std::string &value, const std::string &consistency,
           const int32_t timestamp) override {

    int time_stamp = get_time_in_seconds();
    std::string val_timestamp = value + " " + std::to_string(time_stamp);

    auto it = mem_table.find(key);
    if (it == mem_table.end()) {
      mem_table.insert(std::pair<int16_t, std::string>(key, val_timestamp));
      _return.__set_result("pair created");
    } else {
      it->second = val_timestamp;
      _return.__set_result("pair updated");
    }
    _return.__set_timestamp(time_stamp);
    _return.__set_success(true);
    _return.__set_ip(ip);
    _return.__set_port(port);
  }
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "    [Exception] : Invalid Number of arguments\n "
                 "------------------------------------------------\n"
                 "         usage: ./server <snitch_file>\n "
                 "------------------------------------------------ \n" << std::endl;
    return -1;
  }

  std::string snitch_file = argv[1];

  std::shared_ptr<dkvsHandler> handler(new dkvsHandler(snitch_file));
  std::shared_ptr<TProcessor> processor(new dkvsProcessor(handler));
  std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(handler->port));
  std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);

  std::cout << "\n -----------------------------------------------------\n"
            << "   server is running on: [ " << handler->ip << " : " << handler->port << " ]\n"
            << " -----------------------------------------------------\n\n";
  server.serve();
  return 0;
}

int get_time_in_seconds() {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}


