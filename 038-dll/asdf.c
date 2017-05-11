#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <windows.h>

#include "plugin.h"

int main(void) {
  int a, b;

  HANDLE h = LoadLibrary("myplugin.dll");
  printf("%p\n", (void*)h);

  pPluginOpSupported pOpSupported = NULL;
  pPluginPerformOp pPerformOp = NULL;  

  if (h) {
    pOpSupported = (pPluginOpSupported)GetProcAddress(h, "PluginOpSupported");
    pPerformOp = (pPluginPerformOp)GetProcAddress(h, "PluginPerformOp");    
  }


  puts("Operations:\n"
       " + - / *");

  if (pOpSupported) {
    puts(pOpSupported());
  }

  if (scanf("%i %i\n", &a, &b) != 2) {
    puts("wrong format");
    return 1;
  }

  char op;
  if (scanf("%c", &op) != 1) {
    puts("wrong op");
    return 2;
  }

  int res = 0;
  switch(op)
  {
    case '+': res = a + b; break;
    case '-': res = a - b; break;
    case '*': res = a * b; break;
    case '/': if(b == 0 || (a == INT_MIN && b == -1)) {
                puts("nope");
                return 0;
              } else {
                res = a / b;
              }
              break;
    default:
              if (!pPerformOp) {
                puts("wrong op");
                return 0;                
              }

              if (!pPerformOp(op, a, b, &res)) {
                puts("wrong op");
                return 0;                
              }
              break;
  }

  printf("results: %i\n", res);
  return 0;
}

