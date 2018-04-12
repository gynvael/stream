#include <stdio.h>

template<int v>
struct X {
  static void func() {
    puts("asdf");
  }
};

template<>
struct X<0x30> {
  static void func() {
    puts("xxx");
  }
};

int main(void) {

  X<__TIME__[7]>::func();

  return 0;
}
