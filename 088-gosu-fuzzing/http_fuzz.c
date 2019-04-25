#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "varlst.h"

volatile const char *x;

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  uint8_t *ndata = malloc(size + 1);
  memcpy(ndata, data, size);
  ndata[size] = 0;

  /*if (size == 0) {
    return 0;
  }*/

  varlst *env = varlst_create();
  varlst *http = varlst_create();

  int ret = http_parse((const char*)ndata, (int)size, http, env);
  if (ret != 0) {
    return 0;
  }

  x = varlst_getvalue(env, "asdf");
  x = varlst_getvalue(http, "asdf");

  varlst_clean(env);
  varlst_clean(http);

  varlst_destroy(env);
  varlst_destroy(http);

  free(ndata);

  return 0;
}
