void _start(void* kernel_location) {
  char *textvram = (char*)0xB8000;

  unsigned long long addr = (unsigned long long)kernel_location;
  for (int i = 0; i < 16; i++) {
    textvram[i * 2] = "0123456789ABCDEF"[(addr >> 60) & 0xf];
    addr <<= 4;
  }

  for(;;);
}
