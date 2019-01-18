#pragma once
// NetSock 2 is a helper class for using network sockets with C++.
//
// Version: 2.0.0a
//
// LICENSE
//   Copyright 2018 Gynvael Coldwind
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace netsock {

// Address that can be commonly used for TCP and UDP purposes.
class CommonV4Addr {
 public:
  std::string addr;
  uint16_t port{0};
};

// A pair of addresses. The local address will be a bind address in
// case of outgoing and listening sockets.
class CommonV4AddrPair {
 public:
  CommonV4Addr local;
  CommonV4Addr remote;
};

// std::error_code
class Error {
 public:
  std::string as_text() const;
  // std::string operator()
};

class TCP {
 public:
  enum class SocketType {
    kListen,
    kConnect
  };

  TCP();
  TCP(std::string_view host, uint16_t port);
  TCP(std::string_view host, uint16_t port, SocketType type);
  // TODO: disable copy / etc constructors

  ~TCP();

  void Connect(std::string_view host, uint16_t port);
  void Reconnect();

  void Listen(std::string_view bind_address, uint16_t bind_port);

  int64_t Send(std::string_view data);
  int64_t Send(const std::string& data);
  int64_t Send(const std::vector<std::byte>& data);
  int64_t Send(const void *data, size_t size);

  // TODO(gynvael): Add a decent comment explaining the difference
  // - in non-blocking sockets SendAll blocks
  // see also: SendAllNonBlocking
  // TODO(gynvael) end
  int64_t SendAll(std::string_view data);
  int64_t SendAll(const std::string& data);
  int64_t SendAll(const std::vector<std::byte>& data);
  int64_t SendAll(const void *data, size_t size);

  std::string Recv(size_t size);
  int64_t Recv(void *data, size_t size);

   // TODO(gynvael): Add a decent comment explaining the difference
  // - in non-blocking sockets RecvAll blocks
  // see also: RecvAllNonBlocking
  // TODO(gynvael) end
  std::string RecvAll(size_t size);
  int64_t RecvAll(void *data, size_t size);


  std::string RecvUntil(std::string_view needle);
  std::string RecvUntil(const std::string& needle);
  std::string RecvUntil(const std::vector<std::byte>& needle);
  std::string RecvUntil(void *needle, size_t size);

  // sendall_nonblocking
  // sendall_continue
  // recvall_nonblocking
  // recvuntil_nonblocking

 private:
  int socket_ = -1;

};

// TODO(gynvael): class udp{};

} // netsock

