#include <string.h>
#include <stdlib.h>

int main(void) {
  
  char *x = malloc(1024ULL * 1024ULL * 1024ULL * 1024ULL);
  x[0] = 'a';
  
  return 0;
}

