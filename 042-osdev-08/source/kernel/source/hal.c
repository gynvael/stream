#include "hal.h"

void HAL_PortOutByte(int port, unsigned char v) {
  __asm__("out dx, al\n" : : "a" (v), "d" (port));
}

void HAL_PortOutWord(int port, unsigned short v) {
  __asm__("out dx, ax\n" : : "a" (v), "d" (port));
}

void HAL_PortOutDword(int port, unsigned int v) {
  __asm__("out dx, eax\n" : : "a" (v), "d" (port));
}

void HAL_PauseKernel() {
  // Disable interrupts, park CPU and wait for interrupt. Illogical, but doesn't burn VMs ;)
  __asm__ __volatile__(
    "cli\n"
    "pause\n"
    "hlt\n"
    );
}