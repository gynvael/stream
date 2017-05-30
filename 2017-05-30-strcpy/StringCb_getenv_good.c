#include <stdio.h>
#include <stdlib.h>
#include <Strsafe.h>

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

  // There is also a StringCch* version that operates on 'characters' and
  // not 'bytes'. If sizeof(char) is 1 (which is the default) both sets of
  // functions are identical.
  StringCbCopy(v.buf, sizeof(v.buf), getenv("ASDF"));
  StringCbCat(v.buf, sizeof(v.buf), getenv("FRRR"));

  printf("char buf[8] = \"%s\"\n"
         "int var = 0x%.8x\n", 
         v.buf, v.var);

  return 0;
}

