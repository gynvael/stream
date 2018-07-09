#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>

uint8_t code[] = {
  #include "smc.h"
};

int main(void) {

  typedef int (*func_t)(int, int);
  func_t func = (func_t)code;

  size_t addr = (size_t)code & ~0xfffULL;
  mprotect((void*)addr, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);

  int res = func(1, 2);
  printf("res: %i\n", res);

  return 0;
}

