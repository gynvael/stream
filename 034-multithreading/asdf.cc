#include <thread>
#include <cstdio>
#include <time.h>
volatile unsigned int dd;

void count(unsigned int x, unsigned int y) {
  for (int i = 0; i < 0x70000000 / 100; i++) {
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
    x *= y;
  }

  dd += x;
}

void fcount() {
  double x = 18273.0f;
  for (double i = 0; i < 0x70000000; i++) {
    x *= 12739.0f;
  }
}

#ifndef N
#  define N 1
#endif

int main(void) {
  clock_t a,b;
  a = clock();

  std::thread *th[N];

  for (int i = 0; i < N; i+=1) {
    th[i] = new std::thread(count, 123, 8761);
    //th[i+1] = new std::thread(fcount);
  }

  for (int i = 0; i < N; i++) {
    th[i]->join();
    delete th[i];
  }

  b = clock();

  printf("%u\n", dd);
  printf("%u\n", b-a);


  return 0;
}

