#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct BigNum {
  uint8_t *v; // BCD 1234,,  v = { 4, 3, 2, 1 }

  size_t capacity;
  size_t size; //

} BigNum;

static bool BigNumInit(BigNum *self);

BigNum* BigNumNew() {
  // new BigNum = malloc(sizeof(BigNum)) +
  //              obj->BigNum();
  BigNum *obj = malloc(sizeof(BigNum));
  if (obj == NULL) {
    return NULL;
  }

  if (!BigNumInit(obj)) {
    free(obj);
    return NULL;
  }

  return obj;
}

void BigNumDelete(BigNum *self) {
  if (self == NULL) {
    return;
  }

  free(self->v);
  free(self);
}

char* BigNumToStr(BigNum *self, char *buf, size_t buf_size) {
  if (buf_size == 0 || self->size > buf_size - 1) {
    return NULL;
  }

  // 43215000000000000000000
  // 0
  // 1

  size_t i = self->size - 1;
  while(self->v[i] == 0 && i != 0) {
    i--;
  }

  size_t j = 0;

  for (;;) {
    if (j == buf_size - 1) {
      break;
    }

    buf[j] = self->v[i] + '0';
    j++;

    if (i == 0) {
      break;
    }

    i--;
  }

  buf[j] = '\0';
  return buf;
}

static bool BigNumRealloc(BigNum *self, size_t new_capacity) {
  if (new_capacity <= self->capacity) {
    return true;
  }

  uint8_t *new_v = malloc(new_capacity);
  if (new_v == NULL) {
    return false;
  }

  memcpy(new_v, self->v, self->size);

  free(self->v);
  self->v = new_v;
  self->capacity = new_capacity;

  return true;
}

bool BigNumAssign(BigNum *self, unsigned int u) {
  if (!BigNumRealloc(self, 10)) {
    return false;
  }

  self->v[0] = 0;
  self->size = 0;

  if (u == 0) {
    self->size = 1;
    return true;
  }

  size_t i = 0;
  while (u != 0) {
    self->v[i] = u % 10;
    u /= 10;
    i++;
  }
  self->size = i;

  return true;
}

static bool BigNumInit(BigNum *self) {
  self->v = malloc(16);
  if (self->v == NULL) {
    return false;
  }
  self->capacity = 16;
  self->size = 1;

  self->v[0] = 0;
  return true;
}

// ---
typedef struct BetterNum {
  BigNum *b;
  char buf[1024];
} BetterNum;

BetterNum* BetterNumNew() {
  BetterNum *obj = malloc(sizeof(BetterNum));
  obj->b = BigNumNew();
  return obj;
}

void BetterNumDelete(BetterNum *self) {
  if (self == NULL) {
    return;
  }
  BigNumDelete(self->b);
  free(self);
}

bool BetterNumAssign(BetterNum *self, unsigned int u) {
  return BigNumAssign(self->b, u); // ...
}

char* BetterNumToStr(BetterNum *self) {
  return BigNumToStr(self->b, self->buf, sizeof(self->buf));
}


