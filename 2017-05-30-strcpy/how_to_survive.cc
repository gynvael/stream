#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(void) {
  std::string buf;
  buf += getenv("ASDF");
  buf += getenv("FRRR");

  printf("std::string buf = \"%s\"\n", buf.c_str());
  return 0;
}

