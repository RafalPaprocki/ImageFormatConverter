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


using namespace std;

#undef main //uzywam zeby informacje byly wypisywane do konsoli


SDL_Surface* BmpSurface = NULL;

string saveFileName;
clock_t clockStart, clockEnd;
ifstream rpFile;
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
bool compareR (SDL_Color i,SDL_Color j);
bool compareG (SDL_Color i,SDL_Color j);
bool compareB (SDL_Color i,SDL_Color j);
int findClosestColor(SDL_Color pixel);


SDL_Color fittedPalette[8]; // paleta dopasowana do obrazu

int main(int argc, char* argv[])
{
    char command = '0';
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Wystapil blad inicjalizacji SDL
	{
		cout << "SDL_Init failed:" << endl << SDL_GetError();
		return 1;
	}
	do{
	    cout << "Nacisnij '1' by pobrac i zamienic palete obrazu .bmp na 8bitowa" << endl;
	    cout << "Nacisnij '2' by odczytac obraz .bmp, skompresowac go przy uzyciu LZ77 i zapisac go jako .rprp" << endl;
	    cout << "Nacisnij '3' by odczytac skompresowany obraz .rprp i zapisac go jako .bmp" << endl;
	    /*
	    cout << "Nacisnij '2' by pobrac obraz .bmp i zamienic go na .rprp" << endl;
	    cout << "Nacisnij '3' by odczytac obraz .rprp i zapisac go jako .bmp" << endl;
	    cout << endl;
	    */
		cout << "Nacisnij 'q' lub 'Q' aby wyjsc" << endl;
        cin >> command;
        fflush(stdin);
        switch (command)
        {
            case '1':
                {
                    loadBmp();
                    while( paletteType < 0 || paletteType > 2 ){
                        cout << "Wybierz rodzaj palety w ktorej obraz ma byc zapisany:" << endl;
                        cout <<"[0] Zwykla kolorowa paleta" << endl;
                        cout <<"[1] Paleta dopasowana (MedianCut)" << endl;
                        cout <<"[2] Skala szarosci" << endl;
                        cin >> paletteType;
                        fflush(stdin);
                        //cout <<"Obecna wartosc palettetype = " << paletteType<<endl;
                    }
                    switch(paletteType)
                    {
                    case 0:
                        {
                            BmpNormalPalette();
                            break;
                        }
                    case 1:
                        {
                            MedianCut();
                            BmpFittedPalette();
                            break;
                        }
                    case 2:
                        {
                            BmpGrayScale();
                            break;
                        }
                    default:;
                    }
                    cout << "Podaj nazwe pliku do zapisu (bez rozszerzenia)" << endl;
                    cin >> saveFileName;
                    fflush(stdin);
                    saveFileName += ".bmp";
                    SDL_SaveBMP(BmpSurface,saveFileName.c_str());
                    cout <<"Pomyslnie zapisano plik jako .bmp" << endl << endl;
                    break;
                }


            case '2':
            {
                loadBmp();
                //viewSurfaceInfo();
                while( paletteType < 0 || paletteType > 2 ){
                    cout << "Wybierz rodzaj palety w ktorej obraz ma byc zapisany:" << endl;
                    cout <<"0. Zwykla kolorowa paleta" << endl;
                    cout <<"1. Paleta dopasowana" << endl;
                    cout <<"2. Skala szarosci" << endl;
                    cin >> paletteType;
                    fflush(stdin);
                    //cout <<"Obecna wartosc palettetype = " << paletteType<<endl;
                }
                switch(paletteType)
                {
                case 0:
                    {
                        BmpNormalPalette();
                        break;
                    }
                case 1:
                    {
                        MedianCut();
                        BmpFittedPalette();
                        break;
                    }
                case 2:
                    {
                        BmpGrayScale();
                        break;
                    }
                default:;
                }
                cout << "Podaj nazwe pliku do zapisu (bez rozszerzenia)" << endl;
                cin >> saveFileName;
                fflush(stdin);
                saveFileName += ".rprp";
                RPRP Rprp(BmpSurface);
                //Rprp.show();
                //kompresja
                Rprp.preparePixelsForCompression();

                //przy zapisie pamietaj o aktualizacji rozmiaru pikseli
                fSizeUncomp = sizeof(RPRP::Header) + Rprp.temporaryUncompressedPixels.size() * sizeof(uint8_t);
                cout <<"Rozpoczynam kompresje." << endl;
                clockStart = clock();
                //Rprp.LZ77(Rprp.temporaryUncompressedPixels,Rprp.temporaryUncompressedPixels.size(), Rprp.compressedPixels);

                Rprp.LZ77_Compression(Rprp.temporaryUncompressedPixels, Rprp.compressedPixels);
                clockEnd = clock();
                cout <<"Koncze kompresje." << endl;
                cout <<"Kompresja trwala: " <<((double)(clockEnd - clockStart)) / (double)CLOCKS_PER_SEC << " sekund" << endl;
                fSizeComp = sizeof(RPRP::Header) + Rprp.compressedPixels.size() * sizeof(uint8_t);
                cout <<"Rozmiar pliku po kompresji wynosi: " << fSizeComp << " ,a to jest " << ((double)fSizeComp / (double)fSizeUncomp) * 100 << "% oryginalnego rozmiaru pliku" <<  endl;

                /*
                cout << endl;
                cout <<"Ilosc elementow wektora uncompressedPixels: " << Rprp.uncompressedPixels.size() << endl;
                cout <<"Ilosc elementow wektora temporaryUncompressedPixels: " << Rprp.temporaryUncompressedPixels.size() << endl;
                cout <<"Ilosc elementow wektora compressedPixels: " << Rprp.compressedPixels.size() << endl;
                */
                Rprp.saveCompressedFile(saveFileName.c_str());
                cout <<"Pomyslnie zapisano plik .rprp" << endl << endl;
                break;

                break;
            }
            case '3':
            {
                openRprp();
                //otwieram rprp i odczytuje dane
                RPRP Rprp(rpFile,true);
                //dekompresuje piksele
                //Rprp.LZ77Dekompresja(Rprp.compressedPixels,Rprp.temporaryUncompressedPixels);
                Rprp.LZ77_Decompression(Rprp.temporaryUncompressedPixels, Rprp.compressedPixels, Rprp.header.width_f * Rprp.header.height_f * 3);
                //zamieniam zdekompresowane piksele na format do zapisu w bmp
                Rprp.preparePixelsForSavingBMP();
                //Rprp.show();
                BmpSurface = Rprp.RprpToBmp();
                cout << "Podaj nazwe pliku do zapisu (bez rozszerzenia)" << endl;
                cin >> saveFileName;
                fflush(stdin);
                saveFileName += ".bmp";
                SDL_SaveBMP(BmpSurface,saveFileName.c_str());
                cout <<"Pomyslnie zapisano plik jako .bmp" << endl << endl;
                break;
            }
           
            default:;

        }
	}
    while(command != 'q' && command != 'Q');
    SDL_Quit();
    return 0;
}

void loadBmp() {
	string fileName;
	bool goodInput;
	do{
        do {
            goodInput = true;
            cout << "Podaj nazwe pliku z rozszerzeniem bmp" << endl;
            getline(cin,fileName);
            fflush(stdin);
            if (fileName.length() <= 4) goodInput = false;
            else
			{
			  string filenameEnd = ".bmp";
			  int i = 3;
				while (i >= 0 && filenameEnd[i] == fileName[fileName.length() - 4 + i]) i--;
				if (i >= 0) goodInput = false;
            }

		 } while (goodInput == false);

	BmpSurface = SDL_LoadBMP(fileName.c_str());
	}while (BmpSurface == NULL);

}

void openRprp(){
    string openFileName;
    do{
        cout << "Podaj nazwe pliku z rozszerzeniem .rprp" << endl;
        cin >> openFileName;
        fflush(stdin);

        rpFile.open(openFileName.c_str(), ios::binary);
    }while(!rpFile.good() || openFileName.compare(openFileName.size() - 5, 5, ".rprp") != 0);

    //cout <<"Udalo sie wczytac rprp, wychodze." << endl;
}


void BmpNormalPalette(){
    SDL_Color piksel;
    width=BmpSurface->w;
    height=BmpSurface->h;
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            piksel=getPixel(i,j);
            if(piksel.r>>7)       //jesli przesuniecie bitowe jest 1 to sk³¹dowa wynosi 255, w innym wypadku jest ZEREM
                piksel.r=255;
            else
                piksel.r=0;
            if(piksel.g>>7)
                piksel.g=255;
            else
                piksel.g=0;
            if(piksel.b>>7)
                piksel.b=255;
            else
                piksel.b=0;
            setPixel(i,j,piksel.r,piksel.g,piksel.b);
        }
    }

}

void BmpGrayScale(){
    SDL_Color piksel;
    width=BmpSurface->w;
    height=BmpSurface->h;
    double dww=0;
    int dw=0;
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            piksel=getPixel(i,j);
            dww=0.299*piksel.r+0.587*piksel.g+0.144*piksel.b;                               //ze wzoru
            dw=(int)dww>>5;                                                                 //przesuwamy wartosc wyliczona ze wzoru
            if(dw==8)                                                                       //od 240+ dw bêdzie równe 8, a nie moze byc, wiec dajmy 7
                dw=7;
            switch(dw){                                                                     //wedlug palety ze specyfikacji
                case 0:
                    setPixel(i,j,0,0,0);
                    break;
                case 1:
                    setPixel(i,j,36,36,36);
                    break;
                case 2:
                    setPixel(i,j,72,72,72);
                    break;
                case 3:
                    setPixel(i,j,108,108,108);
                    break;
                case 4:
                     setPixel(i,j,144,144,144);
                    break;
                case 5:
                     setPixel(i,j,181,181,181);
                    break;
                case 6:
                     setPixel(i,j,218,218,218);
                    break;
                case 7:
                     setPixel(i,j,255,255,255);
                    break;
            }
        }
    }
}

void BmpFittedPalette(){
    SDL_Color pixel;
    int indexOfClosestColor;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            pixel = getPixel(i,j);
            indexOfClosestColor = findClosestColor(pixel);
            setPixel(i,j,fittedPalette[indexOfClosestColor].r,fittedPalette[indexOfClosestColor].g,fittedPalette[indexOfClosestColor].b);
        }
    }
}


void MedianCut(){
    vector<SDL_Color>* bucket;
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

    //zczytuje pixele z surface'a do vectora oraz wyznaczam maksymalne i minimalne r,g,b w buckecie
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            pixel = getPixel(i,j);
            bucket[0].push_back(pixel);
            if(pixel.r > upperRed) upperRed = pixel.r;
            if(pixel.g > upperGreen) upperGreen = pixel.g;
            if(pixel.b > upperBlue) upperBlue = pixel.b;
            if(pixel.r < lowerRed) lowerRed = pixel.r;
            if(pixel.g < lowerGreen) lowerGreen = pixel.g;
            if(pixel.b < lowerBlue) lowerBlue = pixel.b;
        }
    }
    rangeRed = upperRed - lowerRed;
    rangeGreen = upperGreen - lowerGreen;
    rangeBlue = upperBlue - lowerBlue;
    maxRange = max(rangeRed,max(rangeGreen, rangeBlue));
    if(maxRange == rangeRed) highestRange = 'r';
    if(maxRange == rangeGreen) highestRange = 'g';
    if(maxRange == rangeBlue) highestRange = 'b';
    sortBucket(bucket[0],highestRange);

    //dziele bucket na polowy
    std::vector<SDL_Color>::iterator it;
    int vectorSize = bucket[0].size();
    int halfIndex;
    //cout << " rozmiar wektora przed podzialem - " << vectorSize;
    halfIndex = vectorSize / 2;
    it = bucket[0].begin() + halfIndex;
    bucket[4].assign(it,bucket[0].end());
    bucket[0].erase(it,bucket[0].end());

    //szukam po czym sortowac powstale 2 buckety i sortuje je

    highestRange = findWhat2SearchInBucket(bucket[0]);
    sortBucket(bucket[0],highestRange);

    highestRange = findWhat2SearchInBucket(bucket[4]);
    sortBucket(bucket[4],highestRange);

    //dziele obydwa buckety na po 2 buckety

    vectorSize = bucket[0].size();
    halfIndex = vectorSize / 2;
    it = bucket[0].begin() + halfIndex;
    bucket[2].assign(it,bucket[0].end());
    bucket[0].erase(it,bucket[0].end());

    vectorSize = bucket[4].size();
    halfIndex = vectorSize / 2;
    it = bucket[4].begin() + halfIndex;
    bucket[6].assign(it,bucket[4].end());
    bucket[4].erase(it,bucket[4].end());

    //szukam po czym sortowac powstale 4 buckety i sortuje je

    highestRange = findWhat2SearchInBucket(bucket[0]);
    sortBucket(bucket[0],highestRange);

    highestRange = findWhat2SearchInBucket(bucket[2]);
    sortBucket(bucket[2],highestRange);

    highestRange = findWhat2SearchInBucket(bucket[4]);
    sortBucket(bucket[4],highestRange);

    highestRange = findWhat2SearchInBucket(bucket[6]);
    sortBucket(bucket[6],highestRange);

    //dziele te 4 buckety na po 2 buckety
    for(int i = 0; i <= 6; i+=2){
        vectorSize = bucket[i].size();
        halfIndex = vectorSize / 2;
        it = bucket[i].begin() + halfIndex;
        bucket[i+1].assign(it,bucket[i].end());
        bucket[i].erase(it, bucket[i].end());
    }

    //sortuje wszystkie 8 bucketow
    for(int i = 0; i < 8; i++){
        highestRange = findWhat2SearchInBucket(bucket[i]);
        sortBucket(bucket[i],highestRange);
    }


    for(int i = 0; i < 8; i++){
        vectorSize = bucket[i].size();
        halfIndex = vectorSize / 2;
        fittedPalette[i] = bucket[i][halfIndex];
    }

}
char findWhat2SearchInBucket(vector<SDL_Color> bucket){
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

    for(int i = 0; i < bucketSize; i++){
        pixel = bucket[i];
        if(pixel.r > upperRed) upperRed = pixel.r;
        if(pixel.g > upperGreen) upperGreen = pixel.g;
        if(pixel.b > upperBlue) upperBlue = pixel.b;
        if(pixel.r < lowerRed) lowerRed = pixel.r;
        if(pixel.g < lowerGreen) lowerGreen = pixel.g;
        if(pixel.b < lowerBlue) lowerBlue = pixel.b;

    }

    rangeRed = upperRed - lowerRed;
    rangeGreen = upperGreen - lowerGreen;
    rangeBlue = upperBlue - lowerBlue;
    maxRange = max(rangeRed,max(rangeGreen, rangeBlue));
    if(maxRange == rangeRed) return 'r';
    if(maxRange == rangeGreen) return 'g';
    if(maxRange == rangeBlue) return 'b';
    return 'x';

}
void sortBucket(vector<SDL_Color>& bucket, char rgb){
    switch(rgb){
        case 'r':
        {
            sort(bucket.begin(),bucket.end(),compareR);
            break;
        }
        case 'g':
        {
            sort(bucket.begin(),bucket.end(),compareG);
            break;
        }
        case 'b':
        {
            sort(bucket.begin(),bucket.end(),compareB);
            break;
        }
        default:;

    }
}

bool compareR (SDL_Color i,SDL_Color j) { return (i.r < j.r); }
bool compareG (SDL_Color i,SDL_Color j) { return (i.g < j.g); }
bool compareB (SDL_Color i,SDL_Color j) { return (i.b < j.b); }

int findClosestColor(SDL_Color pixel){
    int currDiffR;
    int currDiffG;
    int currDiffB;
    int minDiff = INT_MAX;
    int indxMinDiff;

    for(int i = 0; i < 8; i++){
        currDiffR = abs(pixel.r - fittedPalette[i].r);
        currDiffG = abs(pixel.g - fittedPalette[i].g);
        currDiffB = abs(pixel.b - fittedPalette[i].b);
        if((currDiffR + currDiffG + currDiffB) < minDiff){
            minDiff = currDiffR + currDiffG + currDiffB;
            indxMinDiff = i;
        }
    }

    return indxMinDiff;


}
void viewSurfaceInfo(){
    SDL_Surface* bmp = BmpSurface;
    cout <<" format: "<< bmp ->format_version << endl;
    cout <<"h " << bmp->h << endl;
    cout << "w: " <<bmp->w << endl;
    cout <<"flags:" <<  bmp->flags << endl;
    cout << "offset: " <<bmp->offset << endl;
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


