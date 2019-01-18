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
#include "netsock2.h"

namespace netsock {

TCP::TCP() {
}

TCP::TCP(std::string_view host, uint16_t port) {
  Connect(host, port);
}

TCP::TCP(std::string_view host, uint16_t port, SocketType type) {
  switch (type) {
    case kListen: Listen(host, port); break;
    case kConnect: Connect(host, port); break;
  }
}

~TCP::TCP() {
  // TODO
}

void TCP::Connect(std::string_view host, uint16_t port) {
  // TODO: don't user data(), it's not guaranteed that it will be \0 terminated
  // (actually learn how to use this lol)

#ifdef WIN32
  IN_ADDR addr;
#else
  in_addr_t addr;
#endif

  // getaddrinfo
  // freeaddrinfo

  if (inet_pton(AF_INET, host.data(), &addr) != 1) {
    struct hostent *hostip;

    // resolve
    hostip = gethostbyname(host);
    if(!hostip)
      return false;

    memcpy(&ip, hostip->h_addr_list[0], 4);

  }
}




};



