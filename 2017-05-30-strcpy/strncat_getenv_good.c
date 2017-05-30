#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
  // Using struct to "disable" local variable reordering
  // (it's an anti-BO mitigation that prevents a buffer
  // overflow from overwriting local variables that would
  // normally be placed between the buffer and the stack
  // cookie by, well, placing said variable on the other
  // side of the buffer; actually if anyone knows how to
  // disable this from command line do let me know ;).
  struct {
    char buf[8];  
    int var;    
  } v = { "", 0x55555555 };

  strncpy(v.buf, getenv("ASDF"), sizeof(v.buf) - 1);
  v.buf[sizeof(v.buf) - 1] = '\0';

  size_t buf_len = strlen(v.buf);
  if (buf_len < sizeof(v.buf) - 1) {
    size_t buf_left = sizeof(v.buf) - 1 - buf_len;
    strncat(v.buf, getenv("FRRR"), buf_left);  
  }

  printf("char buf[8] = \"%s\"\n"
         "int var = 0x%.8x\n", 
         v.buf, v.var);

  return 0;
}

