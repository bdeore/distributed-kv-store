#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include "dkvs.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <utility>
#include <chrono>
#include <vector>
#include <list>
#include <map>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using std::make_shared;

class replica_node {
 public:
  std::string ip;
  int port, range_begin, range_end;

  replica_node(std::string ip, int port, int range_begin, int range_end)
      : ip(std::move(ip)), port(port), range_begin(range_begin), range_end(range_end) {}
};

class handoff_hint {
  replica_node node;
  int16_t key;
  std::string value;
  int32_t timestamp;

 public:
  handoff_hint(replica_node node, int16_t key, std::string value, int32_t timestamp);
};

class dkvsHandler : virtual public dkvsIf {
 private:
  std::fstream snitch, log;
  std::stringstream node_info;
  std::string line;
  int range_begin, range_end;
  std::vector<replica_node> nodes;
  std::list<handoff_hint> pending_handoff{};
  std::map<int16_t, std::string> mem_table;
  bool log_replay;

  static int get_time_in_seconds();
  void local_get(meta &_return, int16_t key);
  void local_put(meta &_return, int16_t key, const std::string &value, int32_t timestamp);
  void find_forwarding_nodes(int16_t key, std::vector<int> &forwarding_nodes);
  int find_primary_replica(int16_t key);
  void make_request(meta &meta, replica_node node, int16_t key, const std::string &value, int32_t timestamp);
  void get(meta &_return, const int16_t key, const std::string &consistency) override;
  void put(meta &_return, const int16_t key, const std::string &value, const std::string &consistency,
           const int32_t timestamp, const bool is_coordinator) override;
  void request_handoff(const node &n) override;

 public:
  int port;
  std::string ip;
  explicit dkvsHandler(std::string &snitch_file);

};

#endif
