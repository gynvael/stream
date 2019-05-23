#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdint.h>

class FontRenderer {
 private:
  FT_Library ft_library_ = nullptr;
  FT_Face ft_face_ = nullptr;
  uint32_t char_width_ = 0;
  uint32_t char_height_ = 0;

 public:
  static constexpr const char *kDefaultFont =
      "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf";

  ~FontRenderer();

  bool Initialize(uint32_t char_width, uint32_t char_height);
  void RenderGlyph(XImage *surface, uint32_t x, uint32_t y, unsigned long ch,
                   uint8_t r, uint8_t g, uint8_t b);

};





