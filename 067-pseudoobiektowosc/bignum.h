#pragma once
#include <stdbool.h>

typedef void BigNum;

BigNum* BigNumNew();
void BigNumDelete(BigNum *self);
bool BigNumAssign(BigNum *self, unsigned int u);

char* BigNumToStr(BigNum *self, char *buf, size_t buf_size);

typedef void BetterNum;
BetterNum* BetterNumNew();

void BetterNumDelete(BetterNum *self);
bool BetterNumAssign(BetterNum *self, unsigned int u);

char* BetterNumToStr(BetterNum *self);

