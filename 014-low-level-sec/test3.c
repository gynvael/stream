#include <stdlib.h>

int main(void) {
  char *x = malloc(4096 * 4096);
  int i;

  for(i = 0; i < 4096 * 4097; i++) {
    x[i];
  }

  return 0;
}
