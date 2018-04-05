#include <iostream>
#include <SDL/SDL.h>
#include <fstream>
#include <string>
#include <vector>
#include "SDL_Functions.h"

using namespace std;
extern SDL_Surface *BmpSurface;
extern FILE *file;
extern int width;
extern int height;
int power_modulo_fast(int a, int b, int m);


struct RGBPixel
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

extern int paletteType;
extern ifstream rpFile;


// KLASA RPRP

class RPRP{
    public:
    /**********************NAGLOWEK******************/
    // stale 24B
    struct Header{
        char type_f[2];
        unsigned int size_f;
        unsigned int start_f;
        unsigned int width_f;
        unsigned int height_f;
        uint8_t type_p;


    }header;

    /**********************PIKSELE**********************/
    vector<RGBPixel> uncompressedPixels;
    vector<uint8_t> temporaryUncompressedPixels;
    vector<uint8_t> compressedPixels;

    RPRP(SDL_Surface* BmpSurface);
    RPRP(ifstream& rpFile);
    RPRP(ifstream& rpFile, bool compressed);
    void saveFile(string path);
    SDL_Surface* RprpToBmp();
    void show();
    void preparePixelsForCompression();
    void preparePixelsForSavingBMP();
    void saveCompressedFile(string path);

    // kompresja
    int fh(int dl,int indeks, uint8_t tab[]);	//przekazywanie dlugosci ciagu z bufora i indeks pierwszego z przesylanych znakow bufora
    int sprawdzenie(int i,int dl,int indeks_b, uint8_t tab[]);
    bool WyszukiwanieWzorca(int dl,int indeks_b,int indeks_s, uint8_t tab[]);
    void LZ77(vector<uint8_t> dane,int R, vector<uint8_t> &kompresja);
    void LZ77Dekompresja( vector<uint8_t> kompresja,  vector<uint8_t> &dekompresja);
    //kompresja szybsza
    void LZ77_Compression(vector<uint8_t> code, vector<uint8_t> & result);// code - dane wejsciowe, result vector na dane po kompresji
    void LZ77_Decompression(vector<uint8_t> & code, std::vector<uint8_t> & dane, int codesize);
};

