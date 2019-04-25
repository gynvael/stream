#/bin/bash
clang \
  -D_strnicmp=strncasecmp \
  -D_stricmp=strcasecmp \
  linked_list.c string_ext.c http.c http_fuzz.c varlst.c \
  -Wall -Wextra \
  -fsanitize=fuzzer,address \
  -ggdb


