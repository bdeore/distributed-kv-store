#include "server.hpp"

dkvsHandler::dkvsHandler(std::string &snitch_file) {
  bool print = true;
  std::string timestamp, key;
  std::string node_ip, value;
  int node_port, node_range_begin, node_range_end;

  snitch.open(snitch_file);
  if (snitch.is_open()) {
    getline(snitch, line);
    s.str(line);
    s >> node_ip >> node_port >> node_range_begin >> node_range_end;

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
    if (print) {
      std::cout << std::endl << "  creating mem table from log... " << std::endl << std::endl;
      print = false;
    }
    send_handoff_requests = true;
    std::stringstream temp;
    temp.str(line);
    temp >> key >> value >> timestamp;
    meta meta;
    local_put(meta, std::stoi(key), value, std::stoi(timestamp));
  }
  log.close();
  log_replay = false;
}

void dkvsHandler::get(meta &_return, const int16_t key, const std::string &consistency, const bool is_coordinator) {

  if (send_handoff_requests) {
    send_handoff_requests = false;
    process_hints();
    commit_hints();
  }

  std::vector<int> forwarding_nodes;
  std::vector<meta> live_nodes;

  if (is_coordinator) {
    int failed = 0;
    find_forwarding_nodes(key, forwarding_nodes);

    std::string v;
    for (const auto &index:forwarding_nodes) {
      meta meta;
      meta.success = false;
      try {
        make_request(meta, nodes.at(index), key, v, 0, "get");
        if (meta.success) {
          _return.__set_ip(meta.ip);
          _return.__set_port(meta.port);
          _return.__set_success(meta.success);
          _return.__set_timestamp(meta.timestamp);
          _return.__set_result(meta.result);
          if (consistency == "one") break;
          else if (consistency == "quorum") live_nodes.push_back(meta);
        }
      } catch (...) { failed++; }
    }

    if (consistency == "quorum" && !(live_nodes.empty())) {
      auto most_recent = std::max_element(live_nodes.begin(), live_nodes.end(), compare_timestamps_meta);
      _return.__set_ip(most_recent->ip);
      _return.__set_port(most_recent->port);
      _return.__set_success(most_recent->success);
      _return.__set_timestamp(most_recent->timestamp);
      _return.__set_result(most_recent->result);
    }

    if (((failed >= 2) && consistency == "quorum") || ((failed > 2) && consistency == "one")) {
      std::string message = "Exception: [Insufficient Replicas] -- Get Request Failed";
      SystemException exception;
      exception.__set_message(message);
      throw exception;
    }
  } else {
    local_get(_return, key);
  }
}

void dkvsHandler::put(meta &_return, const int16_t key, const std::string &value, const std::string &consistency,
                      const int32_t timestamp, const bool is_coordinator) {

  if (send_handoff_requests) {
    send_handoff_requests = false;
    process_hints();
    commit_hints();
  }

  if (is_coordinator) {
    int failed = 0, time_stamp = get_time_in_seconds();
    std::string val_timestamp = value + " " + std::to_string(time_stamp);
    std::vector<int> forwarding_nodes;

    find_forwarding_nodes(key, forwarding_nodes);
    for (const auto &index:forwarding_nodes) {
      meta meta;
      meta.success = false;
      try {
        make_request(meta, nodes.at(index), key, val_timestamp, time_stamp, "put");
        if (meta.success) {
          std::string dbg =
              nodes.at(index).ip + " " + std::to_string(nodes.at(index).port) + " true " + std::to_string(time_stamp);
          _return.debug.push_back(dbg);
          _return.success = true;
        }
      } catch (...) {
        handoff_hint request(nodes.at(index), key, val_timestamp, time_stamp);
        pending_handoff.push_back(request);
        std::cout << "  -> Stored Hint for Node: [" << nodes.at(index).ip << ":" << nodes.at(index).port << "]"
                  << std::endl;

        std::string dbg =
            nodes.at(index).ip + " " + std::to_string(nodes.at(index).port) + " false " + std::to_string(time_stamp);

        _return.debug.push_back(dbg);
        failed++;
      }
    }

    if (((failed >= 2) && consistency == "quorum") || ((failed > 2) && consistency == "one")) {
      if (consistency == "one") pending_handoff.clear();
      _return.success = false;
      std::string message = "Exception: [Insufficient Replicas] -- Put Request Failed";
      SystemException exception;
      exception.__set_message(message);
      throw exception;
    }
  } else {
    local_put(_return, key, value, timestamp);
  }
}

void dkvsHandler::local_get(meta &_return, int16_t key) {
  std::string value, time_stamp;
  std::stringstream ss;

  auto it = mem_table.find(key);
  if (it == mem_table.end()) {
    _return.__set_result("");
    _return.__set_success(false);
    _return.__set_timestamp(0);
  } else {
    ss << it->second;
    ss >> value >> time_stamp;

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
  std::cout << "  Pair: [" << key << " -> " << value << "] Inserted" << std::endl;

}

void dkvsHandler::make_request(meta &meta, replica_node node, int16_t key, const std::string &value,
                               int32_t timestamp, const std::string &request) {
  if ((node.ip == ip && node.port == port) && request == "put" && !value.empty()) {
    local_put(meta, key, value, timestamp);
  } else if ((node.ip == ip && node.port == port) && request == "get") {
    local_get(meta, key);
  } else {
    auto trans_ep = make_shared<TSocket>(node.ip, node.port);
    trans_ep->setRecvTimeout(15000);
    auto trans_buf = make_shared<TBufferedTransport>(trans_ep);
    auto proto = make_shared<TBinaryProtocol>(trans_buf);
    dkvsClient proxy(proto);
    trans_ep->open();
    if (request == "put") {
      proxy.put(meta, key, value, "a", timestamp, false);
    } else if (request == "get") {
      proxy.get(meta, key, "a", false);
    } else if (request == "handoff") {
      node_info current;
      current.port = port;
      current.ip = ip;
      proxy.request_handoff(current);
    } else if (request == "send_hint") {
      hint h;
      h.key = key;
      h.value = value;
      h.timestamp = timestamp;
      proxy.receive_hint(h);
    }
    trans_ep->close();
  }
}

void dkvsHandler::receive_hint(const hint &h) {
  hints.push_back(h);
  std::cout << "  Hint: " << h.key << " -> " << h.value << std::endl;
}

void dkvsHandler::commit_hints() {
  if (!hints.empty()) {
    std::sort(hints.begin(), hints.end(), compare_timestamps);

    std::cout << std::endl
              << " --------------------------------------------- \n  Sorting Hints\n --------------------------------------------- "
              << std::endl;
    int count = 0;
    for (auto &h:hints) {
      std::cout << "  [ " << ++count << " ] " << h.key << " -> " << h.value << std::endl;
    }

    std::cout << std::endl
              << " --------------------------------------------- \n  Committing Hints\n --------------------------------------------- "
              << std::endl;
    for (auto &h: hints) {
      meta m;
      local_put(m, h.key, h.value, h.timestamp);
    }
    std::cout << std::endl;
    hints.clear();
  }
}

bool dkvsHandler::compare_timestamps(const hint &a, const hint &b) {
  return a.timestamp < b.timestamp;
}

bool dkvsHandler::compare_timestamps_meta(const meta &a, const meta &b) {
  return a.timestamp < b.timestamp;
}

void dkvsHandler::process_hints() {
  for (const auto &node:nodes) {
    if (node.ip == ip && node.port == port) {}
    else {
      meta m;
      make_request(m, node, 0, "", 0, "handoff");
    }
  }
}

void dkvsHandler::request_handoff(const node_info &n) {
  std::cout << "  Handoff Request from: [ " << n.ip << ":" << n.port << " ]";
  std::vector<int> indexes;
  if (!pending_handoff.empty()) {
    int num = -1;
    for (auto &hint:pending_handoff) {
      ++num;
      if (hint.node.ip == n.ip && hint.node.port == n.port) {
        indexes.push_back(num);
        meta m;
        make_request(m, hint.node, hint.key, hint.value, hint.timestamp, "send_hint");
//        std::cout << "made put request to: " << hint.node.ip << " timestamp " << hint.timestamp << " value:"
//                  << hint.value << std::endl;
      }
    }

    for (auto index:indexes) {
      if ((pending_handoff.begin() + index) != pending_handoff.end())
        pending_handoff.erase(pending_handoff.begin() + index);
//      std::cout << "erase successful" << std::endl;
    }
  }

  std::cout << " ...completed!" << std::endl;
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
  std::shared_ptr<ThreadFactory> threadFactory(new ThreadFactory());

  TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);

  std::cout << "\n -----------------------------------------------------\n"
            << "   server is running on: [ " << handler->ip << " : " << handler->port << " ]\n"
            << " -----------------------------------------------------\n\n";

  server.serve();

  return 0;
}

handoff_hint::handoff_hint(replica_node node, int16_t key, std::string value, int32_t timestamp)
    : node(std::move(node)), key(key), value(std::move(value)), timestamp(timestamp) {}
