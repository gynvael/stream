#pragma once
#include <stdint.h>

struct TerminalBackend;
typedef struct TerminalBackend TerminalBackend;

typedef enum TerminalColor {
    TerminalColor_Black                 = 0,
    TerminalColor_Blue                  = 1,
    TerminalColor_Green                 = 2,
    TerminalColor_Cyan                  = 3,
    TerminalColor_Red                   = 4,
    TerminalColor_Magenta               = 5,
    TerminalColor_Brown                 = 6,
    TerminalColor_LightGray             = 7,
    TerminalColor_Gray                  = 8,
    TerminalColor_LightBlue             = 9,
    TerminalColor_LightGreen            = 10,
    TerminalColor_LightCyan             = 11,
    TerminalColor_LightRed              = 12,
    TerminalColor_LightMagenta          = 13,
    TerminalColor_LightYellow           = 14,
    TerminalColor_LightWhite            = 15
} TerminalColor;

// XXX on creation limit size to 32000x32000

typedef void (*TBfunc_SetCursorPosition)(
    TerminalBackend *tb, uint16_t x, uint16_t y);
typedef void (*TBfunc_GetCursorPosition)(
    TerminalBackend *tb, uint16_t *x, uint16_t *y);
typedef void (*TBfunc_ClearScreen)(TerminalBackend *tb);
typedef void (*TBfunc_SetAttribute)(TerminalBackend *tb, TerminalColor bgcolor, TerminalColor fgcolor);
typedef void (*TBfunc_PutCharacter)(TerminalBackend *tb, uint32_t ch);
typedef void (*TBfunc_GetSize)(TerminalBackend *tb,
    uint16_t *w, uint16_t *h);
typedef void (*TBfunc_ScrollLine)(TerminalBackend *tb);

struct TerminalBackend {
  TBfunc_SetCursorPosition func_set_cursor_position;
  TBfunc_GetCursorPosition func_get_cursor_position;
  TBfunc_ClearScreen func_clear_screen;
  TBfunc_SetAttribute func_set_attribute;
  TBfunc_PutCharacter func_put_character;
  TBfunc_GetSize func_get_size;
  TBfunc_ScrollLine func_scroll_line;
  void *backend_data;
};

void T_SetCursorPosition(
    TerminalBackend *tb, uint16_t x, uint16_t y);
void T_GetCursorPosition(
    TerminalBackend *tb, uint16_t *x, uint16_t *y);
void T_PutText(TerminalBackend *tb, const char *s);
void T_SetAttribute(TerminalBackend *tb, TerminalColor bgcolor, TerminalColor fgcolor);
void T_ClearScreen(TerminalBackend *tb);
void T_PutCharacter(TerminalBackend *tb, uint32_t ch);
void T_GetSize(TerminalBackend *tb, uint16_t *w, uint16_t *h);
void T_Printf(TerminalBackend *tb, const char *fmt, ...);
void T_ScrollLine(TerminalBackend *tb);
