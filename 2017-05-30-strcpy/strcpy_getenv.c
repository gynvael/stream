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
  } v = { "", 0 };

  strcpy(v.buf, getenv("ASDF")); // BUFFER OVERFLOW
  printf("char buf[8] = \"%s\"\n"
         "int var = 0x%.8x\n", 
         v.buf, v.var);

  return 0;
}

