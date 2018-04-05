#pragma once
#include <SDL/SDL.h>

extern SDL_Surface *BmpSurface;
extern int width;
extern int height;

void clearScreen(Uint8 R, Uint8 G, Uint8 B);
void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);

