#include <cstdio>
#include "NetSock.h"

void usage() {
  printf("usage: xoxoxo <host> <port>\n");
}

int main(int argc, char **argv) {
  if (argc != 3) {
    usage();
    return 1;
  }

  unsigned short port;
  if (sscanf(argv[2], "%hu", &port) != 1) {
    usage();
    return 2;
  }

  const char *host = argv[1];

  NetSock::InitNetworking();
  NetSock s;
  
  if (!s.Connect(host, port)) {
    fprintf(stderr, "Could not connect!\n");
    return 3;
  }
  
  s.Disconnect();
   
  return 0;
}


