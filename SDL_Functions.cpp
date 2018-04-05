#include "SDL_Functions.h"


SDL_Color getPixel(int x, int y)
{
	SDL_Color color;
	Uint32 col = 0;
	if ((x >= 0) && (x<width) && (y >= 0) && (y<height)) {
		//determine position
		char* pPosition = (char*)BmpSurface->pixels;
		//offset by y
		pPosition += (BmpSurface->pitch*y);
		//offset by x
		pPosition += (BmpSurface->format->BytesPerPixel*x);
		//copy pixel data
		memcpy(&col, pPosition, BmpSurface->format->BytesPerPixel);
		//convert color
		SDL_GetRGB(col, BmpSurface->format, &color.r, &color.g, &color.b);
	}
	return (color);
}

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
	if ((x >= 0) && (x < width) && (y >= 0) && (y < height))
	{
		/* Zamieniamy poszczególne skladowe koloru na format koloru pixela */
		Uint32 pixel = SDL_MapRGB(BmpSurface->format, R, G, B);

		/* Pobieramy informacje ile bajtów zajmuje jeden pixel */
		int bpp = BmpSurface->format->BytesPerPixel;

		/* Obliczamy adres pixela */
		Uint8* p = (Uint8*)BmpSurface->pixels + y * BmpSurface->pitch + x * bpp;

		/* Ustawiamy wartosc pixela, w zaleznosci od formatu powierzchni*/
		switch (bpp)
		{
		case 1: //8-bit
			*p = pixel;
			break;

		case 2: //16-bit
			*(Uint16*)p = pixel;
			break;

		case 3: //24-bit
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;

		case 4: //32-bit
			*(Uint32*)p = pixel;
			break;

		}
		/* update the screen (aka double buffering) */
	}
}
