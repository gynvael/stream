#include <utility>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "textbuffer.h"

void TextBuffer::Resize(uint32_t w, uint32_t h) {
  std::vector<uint32_t> characters;
  std::vector<CharAttrib> attributes;

  const size_t sz = (size_t)w * (size_t)h;
  characters.resize(sz);
  attributes.resize(sz);

  const uint32_t min_w = std::min(w, w_);
  for (uint32_t j = 0; j < std::min(h, h_); j++) {
    memcpy(
      &characters[j * w],
      &characters_[j * w_],
      sizeof(uint32_t) * min_w);

    memcpy(
      &attributes[j * w],
      &attributes_[j * w_],
      sizeof(CharAttrib) * min_w);
  }

  characters_ = std::move(characters);
  attributes_ = std::move(attributes);
  h_ = h;
  w_ = w;

  cursor_x_ = std::min(cursor_x_, w_);
}

TextBuffer::TextBuffer() {
  Reset();
}

TextBuffer::TextBuffer(uint32_t w, uint32_t h) {
  Reset();
  Resize(w, h);
}

void TextBuffer::InsertCharacter(uint32_t character) {
  InsertCharacter(character, current_attribute_);
}

void TextBuffer::LineFeed() {
  cursor_y_++;

  if (cursor_y_ < h_) {
    return;
  }

  // Scroll character and attribute buffers one line up.
  memmove(
      &characters_[0],
      &characters_[1 * w_],
      sizeof(uint32_t) * w_ * (h_ - 1));

  memmove(
      &attributes_[0],
      &attributes_[1 * w_],
      sizeof(CharAttrib) * w_ * (h_ - 1));

  // Clean last line (only chars, leave attributes as is).
  memset(
      &characters_[(h_ - 1) * w_],
      0,
      sizeof(uint32_t) * w_);

  cursor_y_ = h_ - 1;
}

void TextBuffer::CarriageReturn() {
  cursor_x_ = 0;
}

void TextBuffer::WrapCursorIfNeeded() {
  if (cursor_x_ < w_ && cursor_y_ < h_) {
    // No need to scroll or move cursor to new line.
    return;
  }

  if (cursor_x_ >= w_) {
    LineFeed();
    CarriageReturn();
  }
}



void TextBuffer::InsertCharacter(uint32_t character, CharAttrib attribute) {
  WrapCursorIfNeeded();

  size_t idx = cursor_x_ + cursor_y_ * w_;

  characters_[idx] = character;
  attributes_[idx] = attribute;
  putchar(character);

  cursor_x_++;

  WrapCursorIfNeeded();
}

void TextBuffer::ChangeAttribute(CharAttrib attribute) {
  current_attribute_ = attribute;
}

void TextBuffer::ChangeForegroundColor(uint8_t r, uint8_t g, uint8_t b) {
  current_attribute_.fg.r = r;
  current_attribute_.fg.g = g;
  current_attribute_.fg.b = b;
}

void TextBuffer::ChangeBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
  current_attribute_.bg.r = r;
  current_attribute_.bg.g = g;
  current_attribute_.bg.b = b;
}

void TextBuffer::ChangeAuxiliaryAttribute(uint8_t aux) {
  current_attribute_.aux = aux;
}

void TextBuffer::Clear() {
  std::fill(characters_.begin(), characters_.end(), 0);
}

void TextBuffer::Reset() {
  current_attribute_ = kDefaultAttribute;
  w_ = 0;
  h_ = 0;
  cursor_x_ = 0;
  cursor_y_ = 0;
  scroll_y_ = 0;
  Clear();
}

std::pair<uint32_t, uint32_t> TextBuffer::GetCursorPosition() const {
  return std::make_pair(cursor_x_, cursor_y_);
}



