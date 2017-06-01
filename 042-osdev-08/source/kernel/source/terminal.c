#include <stdarg.h>
#include "common.h"
#include "terminal.h"
#include "hal.h"

void T_SetCursorPosition(
    TerminalBackend *tb, uint16_t x, uint16_t y) {
  tb->func_set_cursor_position(tb, x, y);  
}

void T_PutText(TerminalBackend *tb, const char *s) {
  for (; *s != '\0'; s++) {
    switch (*s) {
      case '\n': {
        uint16_t x, y;
        T_GetCursorPosition(tb, &x, &y);
        T_SetCursorPosition(tb, 0, y + 1);
        break;
      }

      case '\r': {
        uint16_t x, y;
        T_GetCursorPosition(tb, &x, &y);
        T_SetCursorPosition(tb, 0, y);
        break;
      }

      case '\t': {
        uint16_t x, y;
        uint16_t sx, sy;
        T_GetCursorPosition(tb, &sx, &sy);
        x = sx; y = sy;

        uint16_t w, h;
        T_GetSize(tb, &w, &h);

        x += 8 - x % 8;

        if (x >= w) {
          x = 0;
          y += 1;
          T_SetCursorPosition(tb, x, y);          
        } else {
          for (uint16_t i = sx; i < x; i++) {
            T_PutCharacter(tb, ' ');
          }
        }
        break;
      }

      default:               
        T_PutCharacter(tb, (unsigned char)*s);
    }
  }
}

void T_ClearScreen(TerminalBackend *tb) {
  tb->func_clear_screen(tb);
}

void T_PutCharacter(TerminalBackend *tb, uint32_t ch) {
  tb->func_put_character(tb, ch);
}

void T_GetCursorPosition(
    TerminalBackend *tb, uint16_t *x, uint16_t *y) {
  tb->func_get_cursor_position(tb, x, y);
}

void T_GetSize(TerminalBackend *tb, uint16_t *w, uint16_t *h) {
  tb->func_get_size(tb, w, h);
}

void T_PrintChar(TerminalBackend *tb, char ch) {
  char buf[2] = { ch, '\0' };
  T_PutText(tb, buf);
}

void T_PrintUInt(TerminalBackend *tb, size_t n) {
  if (n == 0) {
    T_PutCharacter(tb, '0');
    return;
  }

  char buf[24] = {0};
  char *p = &buf[24];
  while (n != 0) {
    --p;
    *p = (n % 10) + '0';
    n /= 10;
  }
  T_PutText(tb, p);  
}

void T_PrintInt(TerminalBackend *tb, long long n) {
  if (n == (-9223372036854775807LL - 1LL)) {
    T_PutText(tb, "-9223372036854775808");
    return;
  }

  if (n < 0) {
    T_PrintChar(tb, '-');
    n = -n;
  }

  if (n == 0) {
    T_PutCharacter(tb, '0');
    return;
  }

  char buf[24] = {0};
  char *p = &buf[23];
  while (n != 0) {
    --p;
    *p = (n % 10) + '0';
    n /= 10;
  }
  T_PutText(tb, p);
}


void T_PrintHex(TerminalBackend *tb, size_t n, int width) {
  if (n == 0) {
    T_PrintChar(tb, '0');
    return;
  }

  int sh = 0;
  while (width < 16 - sh && (n & 0xF000000000000000ULL) == 0) {
    sh ++;
    n <<= 4;
  }

  while (sh < 16) {
    size_t idx = (n & 0xF000000000000000ULL) >> 60;
    T_PrintChar(tb, "0123456789abcdef"[idx]);
    sh ++;
    n <<= 4;
  }
}

extern int
rtl_string_format_v(
    char* buffer,
    size_t buffer_length,
    size_t* processed,
    const char* format,
    va_list arglist
);

void T_Printf(TerminalBackend *tb, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char buffer[512];

  size_t processed = 0;
  int result_code = rtl_string_format_v(buffer, sizeof(buffer), &processed, fmt, args);

  if (result_code == 0 && processed != 0)
  {
    T_PutText(tb, buffer);
  }
  else
  {
    T_PutText(tb, "Internal Terminal Error\n");
    HAL_PauseKernel();
  }

  (void)result_code;
  (void)processed;

  va_end(args);
}


