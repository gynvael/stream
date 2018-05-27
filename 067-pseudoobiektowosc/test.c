#include <stdio.h>
#include "bignum.h"

int main(void) {
  BigNum *a = NULL;
  BetterNum *b = NULL;

  a = BigNumNew();
  if (a == NULL) {
    goto error;
  }

  b = BetterNumNew();
  if (b == NULL) {
    goto error;
  }

  char buf[1024];

  BigNumAssign(a, 1234);
  BetterNumAssign(b, 42);

  printf("a=%s\n", BigNumToStr(a, buf, sizeof(buf)));
  printf("b=%s\n", BetterNumToStr(b));

  BigNumDelete(a);
  BetterNumDelete(b);
  return 0;

error:
  BigNumDelete(a);
  BetterNumDelete(b);
  return 1;
}
