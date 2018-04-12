#include <windows.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

std::string load_shellcode(const char *src) {
  FILE *f = fopen("tmp.asm", "w");
  fwrite(src, 1, strlen(src), f);
  fclose(f);

  system("nasm tmp.asm -o tmp.bin");

  f = fopen("tmp.bin", "r");
  char buf[0x1000]{};
  size_t sz = fread(buf, 1, sizeof(buf), f);
  fclose(f);

  return std::string(buf, sz);
}

int main(int argc, char **argv) {
  if(argc != 2) {
    puts("usage: ppp <pid>");
    return 1;
  }

  int pid = atoi(argv[1]);

  HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
  if(h == nullptr) {
    puts("No luck, cannot open process");
    return 2;
  }

  auto buf = load_shellcode(
  R"EOF(
  [bits 64]
  [org 0x402E00]

  mov al, byte [0x403FFF]
  test al, al
  jnz end

  inc al
  mov byte [0x403FFF], al

  call 0x401560

  end:

  jmp 0x40159B
  )EOF");

  auto jmp = load_shellcode(
  R"EOF(
  [bits 64]
  [org 0x4015C8]
  jmp 0x402E00
  )EOF");

  WriteProcessMemory(h, (void*)0x402E00, buf.c_str(), buf.size(), NULL);
  WriteProcessMemory(h, (void*)0x4015C8, jmp.c_str(), jmp.size(), NULL);


  return 0;
}

