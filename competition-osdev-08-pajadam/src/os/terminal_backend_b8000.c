#include "common.h"
#include "crt.h"
#include "hal.h"
#include "terminal.h"
#include "vga.h"

enum
{
  kVGAControlRegisterIndex_CursorLocationHigh = 0xe,
  kVGAControlRegisterIndex_CursorLocationLow = 0xf,
};

enum
{
  kPortVGAControlRegisterIndex = 0x3d4,
  kPortVGAControlDataRegister = 0x3d5
};

static struct B8000_ContextStruct
{
  unsigned short x;
  unsigned short y;
  unsigned char color;
} B8000_Context;

static void B8000_ScrollLine(TerminalBackend *tb);

static void B8000_SetColor(TerminalBackend *tb, unsigned char color)
{
  B8000_Context.color = color;
}

static void B8000_SetCursorPosition(
    TerminalBackend *tb, uint16_t x, uint16_t y)
{
  UNUSED(tb);

  B8000_Context.x = x;
  B8000_Context.y = y;
}

static void B8000_ClearScreen(TerminalBackend *tb)
{
  UNUSED(tb);
  clearscreen();

  B8000_SetCursorPosition(tb, 0, 0);
}

static void B8000_PutCharacter(TerminalBackend *tb, uint32_t ch)
{
  UNUSED(tb);

  unsigned short x = B8000_Context.x;
  unsigned short y = B8000_Context.y;
  unsigned char color = B8000_Context.color;

  if (y == 25)
  {
    B8000_ScrollLine(tb);
    x = B8000_Context.x;
    y = B8000_Context.y;
  }

  drawfgchar(ch, x, y, color);

  x += 1;
  if (x == 40)
  {
    x = 0;
    y += 1;
  }

  B8000_SetCursorPosition(tb, x, y);
}

static void B8000_GetCursorPosition(
    TerminalBackend *tb, uint16_t *x, uint16_t *y)
{
  UNUSED(tb);
  *x = B8000_Context.x;
  *y = B8000_Context.y;
}

static void B8000_GetSize(TerminalBackend *tb,
                          uint16_t *w, uint16_t *h)
{
  UNUSED(tb);
  *w = 40;
  *h = 25;
}

static void B8000_ScrollLine(TerminalBackend *tb)
{
  unsigned char *mvram = (unsigned char *)0xA0000;
  memmove(mvram, mvram + 320 * 8, 320 * (200 - 1));

  for(int x = 0; x < 40; x++)
  {
    drawchar(' ', x, 25, 0, 0);
  }

  B8000_SetCursorPosition(tb, 0, 25 - 1);
}

static const TerminalBackend B8000_Functions = {
    .func_set_cursor_position = B8000_SetCursorPosition,
    .func_get_cursor_position = B8000_GetCursorPosition,
    .func_clear_screen = B8000_ClearScreen,
    .func_put_character = B8000_PutCharacter,
    .func_get_size = B8000_GetSize,
    .func_scroll_line = B8000_ScrollLine,
    .func_set_color = B8000_SetColor};

TerminalBackend *TerminalBackendB8000(void)
{
  // It's just a function table, so we declared it as const.
  // But we pass it as non-const to the functions.
  return (TerminalBackend *)&B8000_Functions;
}
