#include <cstdio>
#include <vector>
#include <map>
#include <stdint.h>
#include <windows.h>

// Windows conhost palette change test.
// gynvael.coldwind//vx
// Consider this code public domain
// (except the definitions; they are from MSDN; links below).
// Btw, this required Vista/2008 or newer version of windows.
#include <windows.h>
#include <stdio.h>
 
// Uncomment this if CONSOLE_SCREEN_BUFFER_INFOEX is undefined.
// - This seems to be the case in MinGW Platform API.
// - This is NOT the case for the newest Microsoft Windows Platform SDK.
 
// Source for these definitions: MSDN
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms686039(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms683172(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms682091(v=vs.85).aspx

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

  DecompressedXP(const char *filename);

  int32_t w_, h_; 
  std::vector<CHAR_INFO> img_;

  std::map<uint32_t, int> palette_;

 private:
  int AddToPalette(RGB &rgb);
};

int DecompressedXP::AddToPalette(RGB &rgb) {
  uint32_t urgb = rgb.r | (rgb.g << 8) | (rgb.b << 16);
  if (palette_.find(urgb) == palette_.end()) {
    size_t idx = palette_.size();
    palette_[urgb] = idx;
  }

  return palette_[urgb];
}

DecompressedXP::DecompressedXP(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    throw "fail";
  }
  
  fseek(f, 8, SEEK_SET);
  fread(&w_, 1, 4, f);
  fread(&h_, 1, 4, f);

  size_t size = size_t(w_) * h_;
  img_.resize(size);


  for(size_t i = 0; i < size; i++) {
    int32_t ascii_code;
    fread(&ascii_code, 1, 4, f);

    RGB fore, back;
    fread(&fore, 1, 3, f);
    fread(&back, 1, 3, f);

    int fore_idx = AddToPalette(fore);
    int back_idx = AddToPalette(back);

    CHAR_INFO ci;
    ci.Char.AsciiChar = ascii_code;
    ci.Attributes = fore_idx | (back_idx << 4);

    int x = i / h_;
    int y = i % h_;
    int xy = x + y * w_;

    img_[xy] = ci;
  }

  fclose(f);
}

int main(void) {

  HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);

  // Backup.
  CONSOLE_SCREEN_BUFFER_INFOEX bi_backup;
  bi_backup.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
  GetConsoleScreenBufferInfoEx(con, &bi_backup);

  // Image.
  DecompressedXP dxp("xoxoxo1.raw");
  CONSOLE_SCREEN_BUFFER_INFOEX bi;
  bi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

  GetConsoleScreenBufferInfoEx(con, &bi);

  for (std::map<uint32_t,int>::iterator i = dxp.palette_.begin();
       i != dxp.palette_.end();
       ++i) {
    bi.ColorTable[i->second] = i->first;
  }

  SetConsoleScreenBufferInfoEx(con, &bi);

  COORD dxp_c = { dxp.w_, dxp.h_ };
  COORD pos = { 0, 0 };
  SMALL_RECT dst = {
    0, 0, dxp.w_, dxp.h_ 
  };

  WriteConsoleOutput(
      con,
      &dxp.img_[0],
      dxp_c, 
      pos, 
      &dst);

  getchar();

  SetConsoleScreenBufferInfoEx(con, &bi_backup);

  return 0;
}



