#pragma once

#include "font8x8_basic.h"

#define WIDTH 320
#define HEIGHT 200
#define BITS 8

void setpixel(int x, int y, int color);
void clearscreen();
void fillscreen(int color);
void drawchar(unsigned char c, int x, int y, int fgcolor, int bgcolor);
void drawfgchar(unsigned char c, int x, int y, int fgcolor);