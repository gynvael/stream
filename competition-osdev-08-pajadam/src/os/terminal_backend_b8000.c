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
} B8000_Context;

static void B8000_ScrollLine(TerminalBackend *tb);

static void B8000_SetCursorPosition(
    TerminalBackend *tb, uint16_t x, uint16_t y)
{
  UNUSED(tb);
  unsigned int offset = y * 80 + x;

  // ASSERT(offset <= 0xffff);
  // ASSERT(offset <= 80 * 25) <--- w * h konsoli

  // http://wiki.osdev.org/Text_Mode_Cursor#Moving_the_Cursor_with_the_BIOS
  HAL_PortOutByte(kPortVGAControlRegisterIndex,
                  kVGAControlRegisterIndex_CursorLocationLow);
  HAL_PortOutByte(kPortVGAControlDataRegister,
                  offset & 0xff);

  HAL_PortOutByte(kPortVGAControlRegisterIndex,
                  kVGAControlRegisterIndex_CursorLocationHigh);
  HAL_PortOutByte(kPortVGAControlDataRegister,
                  offset >> 8);

  B8000_Context.x = x;
  B8000_Context.y = y;
}

static void B8000_ClearScreen(TerminalBackend *tb)
{
  UNUSED(tb);
  unsigned char *textvram = (unsigned char *)0xB8000;
  for (int i = 0; i < 80 * 25; i++)
  {
    textvram[i * 2 + 0] = ' ';
    textvram[i * 2 + 1] = 0x0A;
  }

  B8000_SetCursorPosition(tb, 0, 0);
}

static void B8000_PutCharacter(TerminalBackend *tb, uint32_t ch)
{
  UNUSED(tb);
  unsigned char *textvram = (unsigned char *)0xB8000;

  unsigned short x = B8000_Context.x;
  unsigned short y = B8000_Context.y;

  if (y == 25)
  {
    B8000_ScrollLine(tb);
    x = B8000_Context.x;
    y = B8000_Context.y;
  }

  unsigned int offset = x + y * 80;
  textvram[offset * 2 + 0] = (unsigned char)ch;
  textvram[offset * 2 + 1] = 0x0A;

  x += 1;
  if (x == 80)
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
  *w = 80;
  *h = 25;
}

static void B8000_ScrollLine(TerminalBackend *tb)
{
  unsigned char *textvram = (unsigned char *)0xB8000;
  memmove(textvram, textvram + 80 * 2, 80 * (25 - 1) * 2);

  for (size_t i = 80 * (25 - 1) * 2; i < 80 * 25 * 2; i += 2)
  {
    textvram[i + 0] = ' ';
    textvram[i + 1] = 0x0A;
  }

  B8000_SetCursorPosition(tb, 0, 25 - 1);
}

static const TerminalBackend B8000_Functions = {
    .func_set_cursor_position = B8000_SetCursorPosition,
    .func_get_cursor_position = B8000_GetCursorPosition,
    .func_clear_screen = B8000_ClearScreen,
    .func_put_character = B8000_PutCharacter,
    .func_get_size = B8000_GetSize,
    .func_scroll_line = B8000_ScrollLine};

TerminalBackend *TerminalBackendB8000(void)
{
  // It's just a function table, so we declared it as const.
  // But we pass it as non-const to the functions.
  return (TerminalBackend *)&B8000_Functions;
}
