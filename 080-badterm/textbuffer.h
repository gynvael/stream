#pragma once
#include <cstdint>
#include <utility>
#include <vector>

struct Color {
  uint8_t r, g, b;
};

struct CharAttrib {
  Color fg;  // Foreground (text) color.
  Color bg;  // Background.
  uint8_t aux; // 1 - bold
};

class TextBuffer {
 private:
  // TODO: if scrolling the buffer after it's full is slow
  // implement a line-based ringbuffer.
  std::vector<uint32_t> characters_;
  std::vector<CharAttrib> attributes_;
  uint32_t w_;  // In characters.
  uint32_t h_;  // In characters.

  const CharAttrib kDefaultAttribute = {
      { 192, 192, 192 },
      { 0, 0, 0 },
      0
  };

  CharAttrib current_attribute_;

  uint32_t cursor_x_;
  uint32_t cursor_y_;  // In scrollback buffer space.
  uint32_t scroll_y_;

  void WrapCursorIfNeeded();  // And scroll if needed too.

 public:
  TextBuffer();
  TextBuffer(uint32_t w, uint32_t h);

  void Resize(uint32_t w, uint32_t h);  // In characters.

  const std::vector<uint32_t>& GetCharacters() const;
  const std::vector<CharAttrib>& GetAttriubutes() const;

  void InsertCharacter(uint32_t character);
  void InsertCharacter(uint32_t character, CharAttrib attribute);
  void LineFeed();
  void CarriageReturn();

  void ChangeAttribute(CharAttrib attribute);
  void ChangeForegroundColor(uint8_t r, uint8_t g, uint8_t b);
  void ChangeBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
  void ChangeAuxiliaryAttribute(uint8_t aux);

  void Clear();
  void Reset();

  std::pair<uint32_t, uint32_t> GetCursorPosition() const;
};



