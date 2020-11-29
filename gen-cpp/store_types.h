/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef store_TYPES_H
#define store_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <functional>
#include <memory>




class meta;

class node_info;

class hint;

typedef struct _meta__isset {
  _meta__isset() : timestamp(false), success(false), ip(false), port(false), result(false), vc(false), response(false) {}
  bool timestamp :1;
  bool success :1;
  bool ip :1;
  bool port :1;
  bool result :1;
  bool vc :1;
  bool response :1;
} _meta__isset;

class meta : public virtual ::apache::thrift::TBase {
 public:

  meta(const meta&);
  meta& operator=(const meta&);
  meta() : timestamp(0), success(0), ip(), port(0), result() {
  }

  virtual ~meta() noexcept;
  int32_t timestamp;
  bool success;
  std::string ip;
  int16_t port;
  std::string result;
  std::vector<node_info>  vc;
  std::vector<bool>  response;

  _meta__isset __isset;

  void __set_timestamp(const int32_t val);

  void __set_success(const bool val);

  void __set_ip(const std::string& val);

  void __set_port(const int16_t val);

  void __set_result(const std::string& val);

  void __set_vc(const std::vector<node_info> & val);

  void __set_response(const std::vector<bool> & val);

  bool operator == (const meta & rhs) const
  {
    if (!(timestamp == rhs.timestamp))
      return false;
    if (!(success == rhs.success))
      return false;
    if (!(ip == rhs.ip))
      return false;
    if (!(port == rhs.port))
      return false;
    if (!(result == rhs.result))
      return false;
    if (!(vc == rhs.vc))
      return false;
    if (!(response == rhs.response))
      return false;
    return true;
  }
  bool operator != (const meta &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const meta & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(meta &a, meta &b);

std::ostream& operator<<(std::ostream& out, const meta& obj);

typedef struct _node_info__isset {
  _node_info__isset() : ip(false), port(false) {}
  bool ip :1;
  bool port :1;
} _node_info__isset;

class node_info : public virtual ::apache::thrift::TBase {
 public:

  node_info(const node_info&);
  node_info& operator=(const node_info&);
  node_info() : ip(), port(0) {
  }

  virtual ~node_info() noexcept;
  std::string ip;
  int16_t port;

  _node_info__isset __isset;

  void __set_ip(const std::string& val);

  void __set_port(const int16_t val);

  bool operator == (const node_info & rhs) const
  {
    if (!(ip == rhs.ip))
      return false;
    if (!(port == rhs.port))
      return false;
    return true;
  }
  bool operator != (const node_info &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const node_info & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(node_info &a, node_info &b);

std::ostream& operator<<(std::ostream& out, const node_info& obj);

typedef struct _hint__isset {
  _hint__isset() : key(false), value(false), timestamp(false) {}
  bool key :1;
  bool value :1;
  bool timestamp :1;
} _hint__isset;

class hint : public virtual ::apache::thrift::TBase {
 public:

  hint(const hint&);
  hint& operator=(const hint&);
  hint() : key(0), value(), timestamp(0) {
  }

  virtual ~hint() noexcept;
  int16_t key;
  std::string value;
  int32_t timestamp;

  _hint__isset __isset;

  void __set_key(const int16_t val);

  void __set_value(const std::string& val);

  void __set_timestamp(const int32_t val);

  bool operator == (const hint & rhs) const
  {
    if (!(key == rhs.key))
      return false;
    if (!(value == rhs.value))
      return false;
    if (!(timestamp == rhs.timestamp))
      return false;
    return true;
  }
  bool operator != (const hint &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const hint & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(hint &a, hint &b);

std::ostream& operator<<(std::ostream& out, const hint& obj);



#endif
