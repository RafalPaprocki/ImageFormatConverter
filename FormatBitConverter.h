#include <iostream>
#include "SDL/SDL.h"
#include "SDL_FUnctions.h"
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <fstream>
#include <time.h>
#include "RPRP.h"

int width;
int height;
int paletteType = 3;
unsigned int fSizeUncomp, fSizeComp;

void loadBmp();
void viewSurfaceInfo();
void openRprp();
void BmpNormalPalette();
void BmpGrayScale();
void BmpFittedPalette();
void MedianCut();
char findWhat2SearchInBucket(vector<SDL_Color> bucket);
void sortBucket(vector<SDL_Color>& bucket, char rgb);
bool compareR(SDL_Color i, SDL_Color j);
bool compareG(SDL_Color i, SDL_Color j);
bool compareB(SDL_Color i, SDL_Color j);
int findClosestColor(SDL_Color pixel);


SDL_Color fittedPalette[8]; // paleta dopasowana do obrazu