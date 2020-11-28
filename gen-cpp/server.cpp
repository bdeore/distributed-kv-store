#include "server.hpp"

dkvsHandler::dkvsHandler(std::string &snitch_file) {
  std::string timestamp, key;
  std::string node_ip, value;
  int node_port, node_range_begin, node_range_end;

  snitch.open(snitch_file);
  if (snitch.is_open()) {
    getline(snitch, line);
    node_info.str(line);
    node_info >> node_ip >> node_port >> node_range_begin >> node_range_end;

    this->ip = node_ip;
    this->port = node_port;
    this->range_begin = node_range_begin;
    this->range_end = node_range_end;

    snitch.close();
    snitch.open("1_snitch.txt");

    while (getline(snitch, line)) {
      std::stringstream temp;
      temp.str(line);
      temp >> node_ip >> node_port >> node_range_begin >> node_range_end;
      replica_node node(node_ip, node_port, node_range_begin, node_range_end);
      nodes.push_back(node);
    }
    snitch.close();
  }

  std::string f = "logs/" + ip;
  log.open(f);
  log_replay = true;
  while (getline(log, line)) {
    std::stringstream temp;
    temp.str(line);
    temp >> key >> value >> timestamp;
    std::cout << "value: " << value << " timestamp: " << timestamp << std::endl;
    meta meta;
    local_put(meta, std::stoi(key), value, std::stoi(timestamp));
  }
  log.close();
  log_replay = false;
}

void dkvsHandler::get(meta &_return, const int16_t key, const std::string &consistency) {
  local_get(_return, key);
}

void dkvsHandler::put(meta &_return, const int16_t key, const std::string &value, const std::string &consistency,
                      const int32_t timestamp, const bool is_coordinator) {
  if (is_coordinator) {
    int failed = 0, time_stamp = get_time_in_seconds();
    std::string val_timestamp = value + " " + std::to_string(time_stamp);
    std::vector<int> forwarding_nodes;

    meta meta;
    find_forwarding_nodes(key, forwarding_nodes);
    for (const auto &index:forwarding_nodes) {
      try {
        make_request(meta, nodes.at(index), key, val_timestamp, time_stamp);
      } catch (...) {
        handoff_hint request(nodes.at(index), key, val_timestamp, time_stamp);
        pending_handoff.push_back(request);
        failed++;
      }
    }
    if (failed >= 2) std::cout << "put failed" << std::endl;
  } else {
    local_put(_return, key, value, timestamp);
  }
}

void dkvsHandler::request_handoff(const node &n) {
  // Your implementation goes here
  printf("request_handoff\n");
}

void dkvsHandler::local_get(meta &_return, int16_t key) {
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

void dkvsHandler::local_put(meta &_return, int16_t key, const std::string &value, int32_t timestamp) {
  auto it = mem_table.find(key);
  if (it == mem_table.end()) {
    std::string val_timestamp = value + " " + std::to_string(timestamp);
    mem_table.insert(std::pair<int16_t, std::string>(key, val_timestamp));
    _return.__set_result("pair created");
  } else {
    std::string val_timestamp = value + " " + std::to_string(timestamp);
    it->second = val_timestamp;
    _return.__set_result("pair updated");
  }
  _return.__set_timestamp(timestamp);
  _return.__set_success(true);
  _return.__set_ip(ip);
  _return.__set_port(port);

  std::string log_entry = std::to_string(key) + " " + value + "\n";

  if (!log_replay) {
    std::string f = "logs/" + ip;

    log.open(f, std::ios::app);
    log << log_entry;
    log.close();
  }

  std::cout << "value inserted: " << value << std::endl;
}

void dkvsHandler::make_request(meta &meta, replica_node node, int16_t key, const std::string &value,
                               int32_t timestamp) {
  if (node.ip == ip && node.port == port) {
    local_put(meta, key, value, timestamp);
  } else {
    auto trans_ep = make_shared<TSocket>(node.ip, node.port);
    trans_ep->setRecvTimeout(15000);
    auto trans_buf = make_shared<TBufferedTransport>(trans_ep);
    auto proto = make_shared<TBinaryProtocol>(trans_buf);
    dkvsClient proxy(proto);
    trans_ep->open();
    proxy.put(meta, key, value, "a", timestamp, false);
    trans_ep->close();
  }
}

void dkvsHandler::find_forwarding_nodes(int16_t key, std::vector<int> &forwarding_nodes) {
  if (!nodes.empty()) {
    int primary = find_primary_replica(key);
    if (primary != -1) {
      for (int i = 0; i < 3; i++) {
        forwarding_nodes.push_back(primary);
        primary = (primary + 1) % 4;
      }
    }
  }
}

int dkvsHandler::find_primary_replica(int16_t key) {
  for (int i = 0; i < (int) nodes.size(); i++) {
    if ((nodes.at(i).range_begin <= key) && (nodes.at(i).range_end >= key)) {
      return i;
    }
  }
  return -1;
}

int dkvsHandler::get_time_in_seconds() {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}

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

handoff_hint::handoff_hint(replica_node node, int16_t key, std::string value, int32_t timestamp)
    : node(std::move(node)), key(key), value(std::move(value)), timestamp(timestamp) {}
