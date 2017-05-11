#pragma once
#include <stdbool.h>

typedef const char* (*pPluginOpSupported)();
typedef bool (*pPluginPerformOp)(char op, int a, int b, int *res);

const char* PluginOpSupported();
bool PluginPerformOp(char op, int a, int b, int *res);


