#include "fontrenderer.h"

FontRenderer::~FontRenderer() {
  if (ft_face_ != nullptr) {
    FT_Done_Face(ft_face_);
    ft_face_ = nullptr;
  }

  if (ft_library_ != nullptr) {
    FT_Done_FreeType(ft_library_);
    ft_library_ = nullptr;
  }
}

bool FontRenderer::Initialize(uint32_t char_width, uint32_t char_height) {
  if (FT_Init_FreeType(&ft_library_) != 0) {
    return false;
  }

  if (FT_New_Face(ft_library_, FontRenderer::kDefaultFont,
                  /*face_index=*/0, &ft_face_) != 0) {
    return false;
  }

  if (FT_Set_Pixel_Sizes(ft_face_, char_width, char_height) != 0) {
    return false;
  }

  char_width_ = char_width;
  char_height_ = char_height;

  return true;
}

void FontRenderer::RenderGlyph(XImage *surface, uint32_t x, uint32_t y, unsigned long ch,
                               uint8_t r, uint8_t g, uint8_t b) {
  FT_Load_Char(ft_face_, ch, FT_LOAD_RENDER);

  FT_GlyphSlot slot = ft_face_->glyph;
  /*printf("[%c] slot->bitmap_left=%i slot->bitmap_top=%i\n",
      ch >= 0x20 && ch <= 0x7e ? (char)ch : '.',
      slot->bitmap_left, slot->bitmap_top);*/

  FT_Bitmap bitmap = slot->bitmap;

  const int kGlyphVerticalOffset = 3;

  uint8_t *data_dst = (uint8_t*)surface->data;
  uint8_t *data_src = bitmap.buffer;
  const int32_t cell_top_i = char_height_ - (int)slot->bitmap_top - kGlyphVerticalOffset;
  const uint32_t cell_top = cell_top_i < 0 ? 0 : cell_top_i;

  for (uint32_t j_dst = cell_top, j_src = 0;
       j_dst < char_height_ && j_src < bitmap.rows; j_dst++, j_src++) {
    for (uint32_t i = 0; i < char_width_ && i < bitmap.width; i++) {
      uint32_t idx_dst = ((x + i) * 4) + (y + j_dst) * surface->bytes_per_line;
      uint32_t idx_src = i + j_src * bitmap.pitch;

      data_dst[idx_dst + 0] = (uint8_t)((data_src[idx_src] * (int)r) / 255);
      data_dst[idx_dst + 1] = (uint8_t)((data_src[idx_src] * (int)g) / 255);
      data_dst[idx_dst + 2] = (uint8_t)((data_src[idx_src] * (int)b) / 255);
    }
  }
}


