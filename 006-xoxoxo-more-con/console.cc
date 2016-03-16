// Windows conhost palette change test.
// gynvael.coldwind//vx
// Consider this code public domain
// (except the definitions; they are from MSDN; links below).
// Btw, this required Vista/2008 or newer version of windows.
#include <cstdio>
#include <stdint.h>

#include "console.h"

// Uncomment this if CONSOLE_SCREEN_BUFFER_INFOEX is undefined.
// - This seems to be the case in MinGW Platform API.
// - This is NOT the case for the newest Microsoft Windows Platform SDK.
 
// Source for these definitions: MSDN
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms686039(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms683172(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms682091(v=vs.85).aspx

void DecompressedXP::SetPalette() {
  HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFOEX bi;
  bi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
  GetConsoleScreenBufferInfoEx(con, &bi);

  for (std::map<uint32_t,int>::iterator i = palette_.begin();
       i != palette_.end();
       ++i) {
    bi.ColorTable[i->second + palette_offset_] = i->first;
  }

  SetConsoleScreenBufferInfoEx(con, &bi);
}

void DecompressedXP::Render(int x, int y) {
  HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);

  COORD dxp_c = { w_, h_ };
  COORD pos = { 0, 0 };
  SMALL_RECT dst = {
    0+x, 0+y, w_+x, h_+y
  };

  WriteConsoleOutput(
      con,
      &img_[0],
      dxp_c, 
      pos, 
      &dst);
}

int DecompressedXP::AddToPalette(RGB &rgb) {
  uint32_t urgb = rgb.r | (rgb.g << 8) | (rgb.b << 16);
  if (palette_.find(urgb) == palette_.end()) {
    size_t idx = palette_.size();
    palette_[urgb] = idx;
  }

  return palette_[urgb] + palette_offset_;
}

DecompressedXP::DecompressedXP(const char *filename, int palette_offset) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    throw "fail";
  }

  palette_offset_ = palette_offset;
  
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

PaletteArchiver::PaletteArchiver() {
  HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
  bi_backup_.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
  GetConsoleScreenBufferInfoEx(con, &bi_backup_);
}

PaletteArchiver::~PaletteArchiver() {
  Restore();
}

void PaletteArchiver::Restore() {
  HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleScreenBufferInfoEx(con, &bi_backup_);
}

#ifdef TEST
int main(void) {

  HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);

  // Backup.
  CONSOLE_SCREEN_BUFFER_INFOEX bi_backup;
  bi_backup.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
  GetConsoleScreenBufferInfoEx(con, &bi_backup);

  // Image.
  DecompressedXP dxp("xoxoxo-title", 3);
  dxp.SetPalette();
  dxp.Render(0, 0);

  getchar();

  SetConsoleScreenBufferInfoEx(con, &bi_backup);

  return 0;
}
#endif




