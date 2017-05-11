#include <stdbool.h>
#include "plugin.h"

const char* PluginOpSupported() {
  return "^";
}

bool PluginPerformOp(char op, int a, int b, int *res) {
  switch (op) {
    case '^':
      *res = a ^ b;
      break;

    default:
      return false;
  }

  return true;
}


