#include "vga.h"

unsigned char *vram = (unsigned char *)0xA0000;

// Set pixel.
void setpixel(int x, int y, int color)
{
    //do not write memory outside the screen buffer, check parameters against the VBE mode info
    if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT)
        return;

    if (x)
        x = (x * (BITS >> 3));
    if (y)
        y = (y * WIDTH);

    register char *cTemp;
    cTemp = &vram[x + y];
    cTemp[0] = color & 0xff;
}

// Fill screen with black color.
void clearscreen()
{
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            setpixel(x, y, 0);
}

// Fill screen with given color.
void fillscreen(int color)
{
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            setpixel(x, y, color);
}

// Draw character with FGColor and BGColor
void drawchar(unsigned char c, int x, int y, int fgcolor, int bgcolor)
{
    int cx, cy;
    int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128};

    for (cx = 0; cx < 8; cx++)
    {
        for (cy = 0; cy < 8; cy++)
        {
            setpixel(
                x * 8 + cx,
                y * 8 + cy,
                font8x8_basic[((int)c)][cy] & mask[cx] ? fgcolor : bgcolor);
        }
    }
}

// Draw character with transparent background
void drawfgchar(unsigned char c, int x, int y, int fgcolor)
{
    int cx, cy;
    int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128};

    for (cx = 0; cx < 8; cx++)
    {
        for (cy = 0; cy < 8; cy++)
        {
            if (font8x8_basic[((int)c)][cy] & mask[cx])
                setpixel(
                    x * 8 + cx,
                    y * 8 + cy,
                    fgcolor
                );
        }
    }
}
