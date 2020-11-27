#include "dkvs.h"
#include "store_types.h"
#include "store_constants.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <memory>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using std::make_shared;

class replica_node;
int generate_random_number();
void generate_prompt(replica_node &coordinator);
void create_node_vector(std::string &snitch_file, std::vector<replica_node> &nodes);
void tokenize_line(std::string &line, std::vector<std::string> &tokens);
void make_request(replica_node &coordinator, std::vector<std::string> &tokens);
bool verbose_output;

class replica_node {
 public:
  std::string ip;
  int port, range_begin, range_end;

  replica_node(std::string ip, int port, int range_begin, int range_end)
      : ip(std::move(ip)), port(port), range_begin(range_begin), range_end(range_end) {}
};

int main(int argc, char *argv[]) {
  std::srand(std::time(nullptr));

  if (argc < 2) {
    std::cout << " [Exception] : Invalid Number of arguments\n " << std::endl;
    return -1;
  }

  std::vector<replica_node> nodes;
  std::string snitch_file = argv[1];

  create_node_vector(snitch_file, nodes);

  int random_number = generate_random_number();
  replica_node coordinator = nodes.at(random_number);
  std::cout << "picked coordinator node: " << coordinator.ip << ":" << coordinator.port << std::endl;

  generate_prompt(coordinator);

}

void generate_prompt(replica_node &coordinator) {

  while (true) {
    std::vector<std::string> tokens;
    std::string line;
    std::cout << "client:> ";

    getline(std::cin, line);
    tokenize_line(line, tokens);

    std::string command = tokens[0];

    if (command == "q" || command == "quit") {
      break;
    }

    make_request(coordinator, tokens);

  }
}

void make_request(replica_node &coordinator, std::vector<std::string> &tokens) {
  auto trans_ep = make_shared<TSocket>(coordinator.ip, coordinator.port);
  auto trans_buf = make_shared<TBufferedTransport>(trans_ep);
  auto proto = make_shared<TBinaryProtocol>(trans_buf);
  dkvsClient client(proto);

  trans_ep->open();
  std::string request, value, consistency;
  int key;

  request = tokens[0];
  key = std::stoi(tokens[1]);

  if (tokens[tokens.size() - 1] == "-v") {
    verbose_output = true;
  }

  meta meta;
  if (request == "get") {
    client.get(meta, key, "A");
    std::cout << meta.result << std::endl;
  } else if (request == "put") {
    value = tokens[2];
    consistency = tokens[3];
    client.put(meta, key, value, "A", 10);
    std::cout << meta.result << std::endl;
  }
  trans_ep->close();
}

void tokenize_line(std::string &line, std::vector<std::string> &tokens) {
  std::string remaining;
  std::stringstream ss;
  ss.str(line);

  while (getline(ss, remaining, ' ')) {
    tokens.push_back(remaining);
  }
}

void create_node_vector(std::string &snitch_file, std::vector<replica_node> &nodes) {
  int node_port, node_range_begin, node_range_end;
  std::string node_ip;
  std::fstream snitch;
  std::string line;

  snitch.open(snitch_file);
  if (snitch.is_open()) {
    while (getline(snitch, line)) {
      std::stringstream temp;
      temp.str(line);
      temp >> node_ip >> node_port >> node_range_begin >> node_range_end;
      replica_node node(node_ip, node_port, node_range_begin, node_range_end);
      nodes.push_back(node);
    }
  }
}

int generate_random_number() {
  return 1 + (rand() >> 6) % 4 - 1;
}

