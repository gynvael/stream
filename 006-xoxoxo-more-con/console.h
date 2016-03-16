#pragma once
#include <vector>
#include <map>
#include <windows.h>

typedef struct _CONSOLE_SCREEN_BUFFER_INFOEX {
  ULONG      cbSize;
  COORD      dwSize;
  COORD      dwCursorPosition;
  WORD       wAttributes;
  SMALL_RECT srWindow;
  COORD      dwMaximumWindowSize;
  WORD       wPopupAttributes;
  BOOL       bFullscreenSupported;
  COLORREF   ColorTable[16];
} CONSOLE_SCREEN_BUFFER_INFOEX, *PCONSOLE_SCREEN_BUFFER_INFOEX;
 
extern "C" BOOL WINAPI GetConsoleScreenBufferInfoEx(
  HANDLE hConsoleOutput,
  PCONSOLE_SCREEN_BUFFER_INFOEX lpConsoleScreenBufferInfoEx
);
 
extern "C" BOOL WINAPI SetConsoleScreenBufferInfoEx(
  HANDLE hConsoleOutput,
  PCONSOLE_SCREEN_BUFFER_INFOEX lpConsoleScreenBufferInfoEx
);

class DecompressedXP {
 public:
  struct RGB {
    uint8_t r, g, b;
  };

  DecompressedXP(const char *filename, int palette_offset);

  void SetPalette();
  void Render(int x, int y);

  int32_t w_, h_; 
  std::vector<CHAR_INFO> img_;

  int palette_offset_;
  std::map<uint32_t, int> palette_;

 private:
  int AddToPalette(RGB &rgb);
};


class PaletteArchiver {
 public:
  PaletteArchiver();
  ~PaletteArchiver();

  void Restore();

 private:
  CONSOLE_SCREEN_BUFFER_INFOEX bi_backup_;
};



