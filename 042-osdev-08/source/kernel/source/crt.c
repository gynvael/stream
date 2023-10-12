#include <stdint.h>
#include "crt.h"

void *memset(void *dst, int c, size_t n) {
  uint8_t *dst_byte = (uint8_t*)dst;
  for (size_t i = 0; i < n; i++) {
    dst_byte[i] = (uint8_t)c;
  }

  return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
  uint8_t *dst_byte = (uint8_t*)dst;  
  const uint8_t *src_byte = (const uint8_t*)src;  
  if (dst < src) {
    for (size_t i = 0; i < n; i++) {
      dst_byte[i] = src_byte[i];
    }
  } else {
    size_t i = n - 1;
    for (;;) {
      dst_byte[i] = src_byte[i];
      if (i == 0) {
        break;
      }
      i--;
    }
  }

  return dst;
}

