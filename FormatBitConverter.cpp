#include "FormatBitConverter.h"

void loadBmp()
{
	string fileName;
	bool goodInput;
	do {
		do {
			goodInput = true;
			cout << "Podaj nazwe pliku z rozszerzeniem bmp" << endl;
			getline(cin, fileName);
			fflush(stdin);

			if (fileName.length() <= 4)
			{
				goodInput = false;
			}

			else
			{
				string filenameEnd = ".bmp";
				int i = 3;
				while (i >= 0 && filenameEnd[i] == fileName[fileName.length() - 4 + i])
				{
					i--;
				}
				if (i >= 0)
				{
					goodInput = false;
				}
			}

		} while (goodInput == false);

		BmpSurface = SDL_LoadBMP(fileName.c_str());

	} while (BmpSurface == NULL);

}

void openRprp()
{
	string openFileName;

	do {
		cout << "Podaj nazwe pliku z rozszerzeniem .rprp" << endl;
		cin >> openFileName;
		fflush(stdin);
		rpFile.open(openFileName.c_str(), ios::binary);

	} while (!rpFile.good() || openFileName.compare(openFileName.size() - 5, 5, ".rprp") != 0);


}


void BmpNormalPalette()
{
	SDL_Color piksel;
	width = BmpSurface->w;
	height = BmpSurface->h;

	for (int i = 0; i<width; i++)
	{
		for (int j = 0; j<height; j++)
		{
			piksel = getPixel(i, j);
			if (piksel.r >> 7)
			{
				piksel.r = 255;
			}
			else
			{
				piksel.r = 0;
			}
			if (piksel.g >> 7)
			{
				piksel.g = 255;
			}
			else
			{
				piksel.g = 0;
			}
			if (piksel.b >> 7)
			{
				piksel.b = 255;
			}
			else
			{
				piksel.b = 0;
			}

			setPixel(i, j, piksel.r, piksel.g, piksel.b);
		}
	}

}

void BmpGrayScale()
{
	SDL_Color piksel;
	width = BmpSurface->w;
	height = BmpSurface->h;

	double dww = 0;
	int dw = 0;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			piksel = getPixel(i, j);
			dww = 0.299 * piksel.r + 0.587 * piksel.g + 0.144 * piksel.b;
			dw = (int)dww >> 5;

			if (dw == 8)
			{
				dw = 7;
			}

			switch (dw)
			{
			case 0:
				setPixel(i, j, 0, 0, 0);
				break;
			case 1:
				setPixel(i, j, 36, 36, 36);
				break;
			case 2:
				setPixel(i, j, 72, 72, 72);
				break;
			case 3:
				setPixel(i, j, 108, 108, 108);
				break;
			case 4:
				setPixel(i, j, 144, 144, 144);
				break;
			case 5:
				setPixel(i, j, 181, 181, 181);
				break;
			case 6:
				setPixel(i, j, 218, 218, 218);
				break;
			case 7:
				setPixel(i, j, 255, 255, 255);
				break;
			}
		}
	}
}

void BmpFittedPalette()
{
	SDL_Color pixel;
	int indexOfClosestColor;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			pixel = getPixel(i, j);
			indexOfClosestColor = findClosestColor(pixel);
			setPixel(i, j, fittedPalette[indexOfClosestColor].r, fittedPalette[indexOfClosestColor].g, fittedPalette[indexOfClosestColor].b);
		}
	}
}


void MedianCut()
{
	vector<SDL_Color> * bucket;
	bucket = new vector<SDL_Color>[8];
	width = BmpSurface->w;
	height = BmpSurface->h;
	SDL_Color pixel;

	int lowerRed = INT_MAX;
	int lowerGreen = INT_MAX;
	int lowerBlue = INT_MAX;
	int upperRed = 0;
	int upperGreen = 0;
	int upperBlue = 0;
	int rangeRed;
	int rangeGreen;
	int rangeBlue;
	int maxRange;
	char highestRange;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			pixel = getPixel(i, j);
			bucket[0].push_back(pixel);
			if (pixel.r > upperRed) upperRed = pixel.r;
			if (pixel.g > upperGreen) upperGreen = pixel.g;
			if (pixel.b > upperBlue) upperBlue = pixel.b;
			if (pixel.r < lowerRed) lowerRed = pixel.r;
			if (pixel.g < lowerGreen) lowerGreen = pixel.g;
			if (pixel.b < lowerBlue) lowerBlue = pixel.b;
		}
	}

	rangeRed = upperRed - lowerRed;
	rangeGreen = upperGreen - lowerGreen;
	rangeBlue = upperBlue - lowerBlue;
	maxRange = max(rangeRed, max(rangeGreen, rangeBlue));

	if (maxRange == rangeRed) highestRange = 'r';
	if (maxRange == rangeGreen) highestRange = 'g';
	if (maxRange == rangeBlue) highestRange = 'b';

	sortBucket(bucket[0], highestRange);

	std::vector<SDL_Color>::iterator it;
	int vectorSize = bucket[0].size();
	int halfIndex;

	halfIndex = vectorSize / 2;
	it = bucket[0].begin() + halfIndex;
	bucket[4].assign(it, bucket[0].end());
	bucket[0].erase(it, bucket[0].end());

	highestRange = findWhat2SearchInBucket(bucket[0]);
	sortBucket(bucket[0], highestRange);

	highestRange = findWhat2SearchInBucket(bucket[4]);
	sortBucket(bucket[4], highestRange);

	vectorSize = bucket[0].size();
	halfIndex = vectorSize / 2;
	it = bucket[0].begin() + halfIndex;
	bucket[2].assign(it, bucket[0].end());
	bucket[0].erase(it, bucket[0].end());

	vectorSize = bucket[4].size();
	halfIndex = vectorSize / 2;
	it = bucket[4].begin() + halfIndex;
	bucket[6].assign(it, bucket[4].end());
	bucket[4].erase(it, bucket[4].end());

	highestRange = findWhat2SearchInBucket(bucket[0]);
	sortBucket(bucket[0], highestRange);

	highestRange = findWhat2SearchInBucket(bucket[2]);
	sortBucket(bucket[2], highestRange);

	highestRange = findWhat2SearchInBucket(bucket[4]);
	sortBucket(bucket[4], highestRange);

	highestRange = findWhat2SearchInBucket(bucket[6]);
	sortBucket(bucket[6], highestRange);

	for (int i = 0; i <= 6; i += 2)
	{
		vectorSize = bucket[i].size();
		halfIndex = vectorSize / 2;
		it = bucket[i].begin() + halfIndex;
		bucket[i + 1].assign(it, bucket[i].end());
		bucket[i].erase(it, bucket[i].end());
	}

	for (int i = 0; i < 8; i++)
	{
		highestRange = findWhat2SearchInBucket(bucket[i]);
		sortBucket(bucket[i], highestRange);
	}

	for (int i = 0; i < 8; i++)
	{
		vectorSize = bucket[i].size();
		halfIndex = vectorSize / 2;
		fittedPalette[i] = bucket[i][halfIndex];
	}

}
char findWhat2SearchInBucket(vector<SDL_Color> bucket)
{
	SDL_Color pixel;
	int bucketSize = bucket.size();
	int lowerRed = INT_MAX;
	int lowerGreen = INT_MAX;
	int lowerBlue = INT_MAX;
	int upperRed = 0;
	int upperGreen = 0;
	int upperBlue = 0;
	int rangeRed;
	int rangeGreen;
	int rangeBlue;
	int maxRange;

	for (int i = 0; i < bucketSize; i++)
	{
		pixel = bucket[i];
		if (pixel.r > upperRed) upperRed = pixel.r;
		if (pixel.g > upperGreen) upperGreen = pixel.g;
		if (pixel.b > upperBlue) upperBlue = pixel.b;
		if (pixel.r < lowerRed) lowerRed = pixel.r;
		if (pixel.g < lowerGreen) lowerGreen = pixel.g;
		if (pixel.b < lowerBlue) lowerBlue = pixel.b;

	}

	rangeRed = upperRed - lowerRed;
	rangeGreen = upperGreen - lowerGreen;
	rangeBlue = upperBlue - lowerBlue;
	maxRange = max(rangeRed, max(rangeGreen, rangeBlue));

	if (maxRange == rangeRed) return 'r';
	if (maxRange == rangeGreen) return 'g';
	if (maxRange == rangeBlue) return 'b';

	return 'x';

}
void sortBucket(vector<SDL_Color>& bucket, char rgb)
{
	switch (rgb)
	{
	case 'r':
	{
		sort(bucket.begin(), bucket.end(), compareR);
		break;
	}
	case 'g':
	{
		sort(bucket.begin(), bucket.end(), compareG);
		break;
	}
	case 'b':
	{
		sort(bucket.begin(), bucket.end(), compareB);
		break;
	}
	default:;

	}
}

bool compareR(SDL_Color i, SDL_Color j)
{
	return (i.r < j.r);
}
bool compareG(SDL_Color i, SDL_Color j)
{
	return (i.g < j.g);
}
bool compareB(SDL_Color i, SDL_Color j)
{
	return (i.b < j.b);
}

int findClosestColor(SDL_Color pixel)
{
	int currDiffR;
	int currDiffG;
	int currDiffB;
	int minDiff = INT_MAX;
	int indxMinDiff;

	for (int i = 0; i < 8; i++)
	{
		currDiffR = abs(pixel.r - fittedPalette[i].r);
		currDiffG = abs(pixel.g - fittedPalette[i].g);
		currDiffB = abs(pixel.b - fittedPalette[i].b);

		if ((currDiffR + currDiffG + currDiffB) < minDiff)
		{
			minDiff = currDiffR + currDiffG + currDiffB;
			indxMinDiff = i;
		}
	}

	return indxMinDiff;

}
void viewSurfaceInfo()
{
	SDL_Surface* bmp = BmpSurface;
	cout << " format: " << bmp->format_version << endl;
	cout << "h " << bmp->h << endl;
	cout << "w: " << bmp->w << endl;
	cout << "flags:" << bmp->flags << endl;
	cout << "offset: " << bmp->offset << endl;
	cout << "colorkey" << bmp->format->colorkey << endl;
	cout << "palette" << bmp->format->palette << endl;

	cout << (int)bmp->format->Rmask << endl;
	cout << (int)bmp->format->Gmask << endl;
	cout << (int)bmp->format->Bmask << endl;
	cout << (int)bmp->format->Amask << endl;
	cout << (int)bmp->format->Rshift << endl;
	cout << (int)bmp->format->Gshift << endl;
	cout << (int)bmp->format->Bshift << endl;
	cout << (int)bmp->format->Ashift << endl;
	cout << "Bytes: " << (int)bmp->format->BytesPerPixel << endl;
	cout << "Bits" << (int)bmp->format->BitsPerPixel << endl;
	cout << (int)bmp->format->Rloss << endl;
	cout << (int)bmp->format->Aloss << endl;
	cout << (int)bmp->format->Gloss << endl;
	cout << (int)bmp->format->Bloss << endl;
	cout << (int)bmp->format->alpha << endl;

	cout << "Pitch: " << bmp->pitch << endl;
}
