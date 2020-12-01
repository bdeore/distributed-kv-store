#include "dkvs.h"
#include "store_types.h"
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
#include <iomanip>
#include <chrono>

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using std::make_shared;

class replica_node;
int generate_random_number();
void generate_prompt(replica_node &coordinator);
void create_node_vector(std::string &snitch_file, std::vector<replica_node> &nodes);
void tokenize_line(std::string &line, std::vector<std::string> &tokens);
void make_request(replica_node &coordinator, std::vector<std::string> &tokens);
void print_results(meta &meta, int key);
void print_metadata(meta &meta, const std::string &request);
void print_menu();

bool verbose_output;
std::vector<replica_node> nodes;

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

  std::string snitch_file = argv[1];

  create_node_vector(snitch_file, nodes);

  int random_number = generate_random_number();
  replica_node coordinator = nodes.at(random_number);

  std::cout << " ------------------------------------------------\n"
            << "    coordinator node: " << coordinator.ip << ":" << coordinator.port << "\n"
            << " ------------------------------------------------ \n" << std::endl;

  generate_prompt(coordinator);

}

void generate_prompt(replica_node &coordinator) {
  while (true) {
    std::vector<std::string> tokens;
    std::string line;
    std::cout << " client:> ";

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
  std::string request, value, consistency;
  request = tokens[0];
  int key;

  if (tokens[tokens.size() - 1] == "-v") {
    verbose_output = true;
  }

  if (request == "select" && tokens.size() == 2) {
    coordinator = nodes.at(std::stoi(tokens[1]) - 1);
    std::cout << " New coordinator is: [ " << coordinator.ip << " : " << coordinator.port << " ]\n" << std::endl;
  }

  auto trans_ep = make_shared<TSocket>(coordinator.ip, coordinator.port);
  auto trans_buf = make_shared<TBufferedTransport>(trans_ep);
  auto proto = make_shared<TBinaryProtocol>(trans_buf);
  dkvsClient client(proto);
  trans_ep->open();

  std::string op_consistency = "quorum";

  if (request == "get" && tokens.size() >= 2) {
    meta meta;
    key = std::stoi(tokens[1]);

    if (tokens.size() >= 4 && tokens[2] == "-c") {
      op_consistency = tokens[3];
      if (op_consistency == "q" || op_consistency == "Q" || op_consistency == "quorum")
        op_consistency = "quorum";
      else if (op_consistency == "o" || op_consistency == "O" || op_consistency == "one")
        op_consistency = "one";
      else
        std::cout << " Invalid value of Consistency" << std::endl;
    }

    try {
      client.get(meta, key, op_consistency, true);
    } catch (SystemException &s) {
      client.get(meta, key, op_consistency, true);
    }

    print_results(meta, key);
    if (verbose_output) print_metadata(meta, "get");
  } else if ((request == "put") && tokens.size() >= 3) {
    meta meta;
    key = std::stoi(tokens[1]);
    value = tokens[2];

    if (tokens.size() >= 5 && tokens[3] == "-c") {
      op_consistency = tokens[4];
      if (op_consistency == "q" || op_consistency == "Q" || op_consistency == "quorum")
        op_consistency = "quorum";
      else if (op_consistency == "o" || op_consistency == "O" || op_consistency == "one")
        op_consistency = "one";
      else
        std::cout << "  Invalid value of Consistency" << std::endl;
    }

    try {
      client.put(meta, key, value, op_consistency, 0, true);
    } catch (SystemException &s) {
      client.put(meta, key, value, op_consistency, 0, true);
    }

    if (verbose_output) print_metadata(meta, "put");
  } else {
    if (tokens[0] != "select") {
      if (tokens[0] != "help") {
        {
          std::cout << " Invalid [Command|Format] : ";
          for (const auto &token:tokens) {
            std::cout << token << " ";
          }
          std::cout << std::endl;
        }
      }
      print_menu();
    }
  }
  verbose_output = false;
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

void print_metadata(meta &meta, const std::string &request) {

  if (request == "get") {
    std::cout << " ----------------------------------------------------\n"
              << "   Received Metadata: "
              << " \n ----------------------------------------------------" << std::endl;
    time_t time(meta.timestamp);

    if (meta.success) {
      std::cout << "   Server    : " << meta.ip << ":" << meta.port << "\n"
                << "   Timestamp : " << std::put_time(std::localtime(&time), "%c %Z") << "\n"
                << " ----------------------------------------------------\n"
                << std::endl;
    } else std::cout << std::endl;
  } else if (request == "put") {
    std::cout << " -------------------------------------------------------------\n"
              << "   " << ((meta.success) ? "Put Successful - " : "Put Failed - ") << "Received Metadata: "
              << " \n -------------------------------------------------------------" << std::endl;

    for (auto d: meta.debug) {
      std::stringstream ss;
      std::string ip, success;
      int32_t port, tp;
      ss << d;
      ss >> ip >> port >> success >> tp;
      time_t time(tp);

      if (success == "false") {
        std::cout << "  [" << ((success == "true") ? "\u2713" : "X") << "] " << ip << ":" << port << " ->  "
                  << "Node Failed " << std::endl;
      } else {
        std::cout << "  [" << ((success == "true") ? "\u2713" : "X") << "] " << ip << ":" << port << " ->  "
                  << std::put_time(std::localtime(&time), "%c %Z") << " " << std::endl;
      }
    }
    std::cout << std::endl;
  }
}

void print_results(meta &meta, int key) {
  std::cout << " ---------------------------------\n"
            << "   Key " << ((meta.success) ? "Exists" : "Does Not Exist")
            << " \n ---------------------------------" << std::endl;
  if (meta.success) {
    std::cout << "   key   : " << key << "\n"
              << "   value : " << meta.result << "\n"
              << " ---------------------------------\n"
              << std::endl;
  } else std::cout << std::endl;
}

void print_menu() {
  std::cout << std::endl << " ---------------------------------------------------------\n"
            << "                     commands supported\n"
            << " ---------------------------------------------------------\n"
            << "   put <key> <value> [-c <one|o|O> | <quorum|q|Q> ] [-v]\n"
            << "   get <key> [-c <one|o|O> | <quorum|q|Q> ] [-v]\n"
            << "   select <coordinator node>\n"
            << " ---------------------------------------------------------\n" << std::endl;

}
