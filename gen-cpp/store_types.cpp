/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "store_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>




meta::~meta() noexcept {
}


void meta::__set_timestamp(const int32_t val) {
  this->timestamp = val;
}

void meta::__set_success(const bool val) {
  this->success = val;
}

void meta::__set_ip(const std::string& val) {
  this->ip = val;
}

void meta::__set_port(const int16_t val) {
  this->port = val;
}

void meta::__set_result(const std::string& val) {
  this->result = val;
}

void meta::__set_debug(const std::vector<std::string> & val) {
  this->debug = val;
}
std::ostream& operator<<(std::ostream& out, const meta& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t meta::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->timestamp);
          this->__isset.timestamp = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->success);
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->ip);
          this->__isset.ip = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I16) {
          xfer += iprot->readI16(this->port);
          this->__isset.port = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->result);
          this->__isset.result = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->debug.clear();
            uint32_t _size0;
            ::apache::thrift::protocol::TType _etype3;
            xfer += iprot->readListBegin(_etype3, _size0);
            this->debug.resize(_size0);
            uint32_t _i4;
            for (_i4 = 0; _i4 < _size0; ++_i4)
            {
              xfer += iprot->readString(this->debug[_i4]);
            }
            xfer += iprot->readListEnd();
          }
          this->__isset.debug = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t meta::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("meta");

  xfer += oprot->writeFieldBegin("timestamp", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->timestamp);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_BOOL, 2);
  xfer += oprot->writeBool(this->success);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("ip", ::apache::thrift::protocol::T_STRING, 3);
  xfer += oprot->writeString(this->ip);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("port", ::apache::thrift::protocol::T_I16, 4);
  xfer += oprot->writeI16(this->port);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("result", ::apache::thrift::protocol::T_STRING, 5);
  xfer += oprot->writeString(this->result);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("debug", ::apache::thrift::protocol::T_LIST, 6);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRING, static_cast<uint32_t>(this->debug.size()));
    std::vector<std::string> ::const_iterator _iter5;
    for (_iter5 = this->debug.begin(); _iter5 != this->debug.end(); ++_iter5)
    {
      xfer += oprot->writeString((*_iter5));
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(meta &a, meta &b) {
  using ::std::swap;
  swap(a.timestamp, b.timestamp);
  swap(a.success, b.success);
  swap(a.ip, b.ip);
  swap(a.port, b.port);
  swap(a.result, b.result);
  swap(a.debug, b.debug);
  swap(a.__isset, b.__isset);
}

meta::meta(const meta& other6) {
  timestamp = other6.timestamp;
  success = other6.success;
  ip = other6.ip;
  port = other6.port;
  result = other6.result;
  debug = other6.debug;
  __isset = other6.__isset;
}
meta& meta::operator=(const meta& other7) {
  timestamp = other7.timestamp;
  success = other7.success;
  ip = other7.ip;
  port = other7.port;
  result = other7.result;
  debug = other7.debug;
  __isset = other7.__isset;
  return *this;
}
void meta::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "meta(";
  out << "timestamp=" << to_string(timestamp);
  out << ", " << "success=" << to_string(success);
  out << ", " << "ip=" << to_string(ip);
  out << ", " << "port=" << to_string(port);
  out << ", " << "result=" << to_string(result);
  out << ", " << "debug=" << to_string(debug);
  out << ")";
}


node_info::~node_info() noexcept {
}


void node_info::__set_ip(const std::string& val) {
  this->ip = val;
}

void node_info::__set_port(const int16_t val) {
  this->port = val;
}
std::ostream& operator<<(std::ostream& out, const node_info& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t node_info::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->ip);
          this->__isset.ip = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I16) {
          xfer += iprot->readI16(this->port);
          this->__isset.port = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t node_info::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("node_info");

  xfer += oprot->writeFieldBegin("ip", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->ip);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("port", ::apache::thrift::protocol::T_I16, 2);
  xfer += oprot->writeI16(this->port);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(node_info &a, node_info &b) {
  using ::std::swap;
  swap(a.ip, b.ip);
  swap(a.port, b.port);
  swap(a.__isset, b.__isset);
}

node_info::node_info(const node_info& other8) {
  ip = other8.ip;
  port = other8.port;
  __isset = other8.__isset;
}
node_info& node_info::operator=(const node_info& other9) {
  ip = other9.ip;
  port = other9.port;
  __isset = other9.__isset;
  return *this;
}
void node_info::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "node_info(";
  out << "ip=" << to_string(ip);
  out << ", " << "port=" << to_string(port);
  out << ")";
}


hint::~hint() noexcept {
}


void hint::__set_key(const int16_t val) {
  this->key = val;
}

void hint::__set_value(const std::string& val) {
  this->value = val;
}

void hint::__set_timestamp(const int32_t val) {
  this->timestamp = val;
}
std::ostream& operator<<(std::ostream& out, const hint& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t hint::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I16) {
          xfer += iprot->readI16(this->key);
          this->__isset.key = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->value);
          this->__isset.value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->timestamp);
          this->__isset.timestamp = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t hint::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("hint");

  xfer += oprot->writeFieldBegin("key", ::apache::thrift::protocol::T_I16, 1);
  xfer += oprot->writeI16(this->key);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("value", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->value);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("timestamp", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32(this->timestamp);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(hint &a, hint &b) {
  using ::std::swap;
  swap(a.key, b.key);
  swap(a.value, b.value);
  swap(a.timestamp, b.timestamp);
  swap(a.__isset, b.__isset);
}

hint::hint(const hint& other10) {
  key = other10.key;
  value = other10.value;
  timestamp = other10.timestamp;
  __isset = other10.__isset;
}
hint& hint::operator=(const hint& other11) {
  key = other11.key;
  value = other11.value;
  timestamp = other11.timestamp;
  __isset = other11.__isset;
  return *this;
}
void hint::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "hint(";
  out << "key=" << to_string(key);
  out << ", " << "value=" << to_string(value);
  out << ", " << "timestamp=" << to_string(timestamp);
  out << ")";
}


SystemException::~SystemException() noexcept {
}


void SystemException::__set_message(const std::string& val) {
  this->message = val;
__isset.message = true;
}
std::ostream& operator<<(std::ostream& out, const SystemException& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t SystemException::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->message);
          this->__isset.message = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t SystemException::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("SystemException");

  if (this->__isset.message) {
    xfer += oprot->writeFieldBegin("message", ::apache::thrift::protocol::T_STRING, 1);
    xfer += oprot->writeString(this->message);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(SystemException &a, SystemException &b) {
  using ::std::swap;
  swap(a.message, b.message);
  swap(a.__isset, b.__isset);
}

SystemException::SystemException(const SystemException& other12) : TException() {
  message = other12.message;
  __isset = other12.__isset;
}
SystemException& SystemException::operator=(const SystemException& other13) {
  message = other13.message;
  __isset = other13.__isset;
  return *this;
}
void SystemException::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "SystemException(";
  out << "message="; (__isset.message ? (out << to_string(message)) : (out << "<null>"));
  out << ")";
}

const char* SystemException::what() const noexcept {
  try {
    std::stringstream ss;
    ss << "TException - service has thrown: " << *this;
    this->thriftTExceptionMessageHolder_ = ss.str();
    return this->thriftTExceptionMessageHolder_.c_str();
  } catch (const std::exception&) {
    return "TException - service has thrown: SystemException";
  }
}


