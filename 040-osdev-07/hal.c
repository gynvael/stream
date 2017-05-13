#include "hal.h"

void HAL_PortOutByte(int port, unsigned char v) {
  __asm("out dx, al\n" : : "a" (v), "d" (port));
}

void HAL_PortOutWord(int port, unsigned short v) {
  __asm("out dx, ax\n" : : "a" (v), "d" (port));
}

void HAL_PortOutDword(int port, unsigned int v) {
  __asm("out dx, eax\n" : : "a" (v), "d" (port));
}

