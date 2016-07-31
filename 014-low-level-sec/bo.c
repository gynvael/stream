#include <stdio.h>

// RSP --> [buf] [??] [ret] [f] [???] [ret] ... [argc] [argv[0]] [env[0]] ... auxv |

void myfunc(FILE *f) {
  char buf[1024];
  fread(buf, 1, 2048, f);
  puts(buf);
}

int main(int argc, char **argv) {
  FILE *f = fopen(argv[1], "rb");
  myfunc(f);
  fclose(f);

  return 0;
}
