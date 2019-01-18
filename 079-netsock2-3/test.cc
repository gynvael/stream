#include <string_view>
#include <string>
#include <cstdio>
#include "netsock2.h"


int main(void) {
  //WSANOTINITIALISED
  // 10093
  // printf("error: %i\n", WSAGetLastError());


  netsock::TCP s("gynvael.coldwind.pl", 80);
  s.SendAll(
      "GET / HTTP/1.1\r\n"
      "Host: coldwind.pl\r\n"
      "\r\n");

  printf("Received:\n%s\n",
      s.RecvUntil("\r\n\r\n").c_str());





  //auto [addr, ip] = std::make_pair<const char*, int>("asdf", 123);


  //netsock::TCP client("gynvael.coldwind.pl", 123);


}

