#include<stdio.h>
#include<windows.h>

void func() {
  puts("udalo sie!");
}

int main(void) {
  printf("%p\n", (void*)func);

  for(;;) {
    putchar('.');
    fflush(stdout);
    Sleep(1000);
  }

  return 0;
}

