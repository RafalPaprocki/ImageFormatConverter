#include "RPRP.h"

using namespace std;
 RPRP::RPRP(SDL_Surface* BmpSurface){
    header.type_f[0] = 'R';
    header.type_f[1] = 'P';
    header.height_f = BmpSurface->h;
    header.width_f = BmpSurface->w;
    header.start_f = sizeof(Header); //poki co sam header
    header.type_p = static_cast<uint8_t>(paletteType);
    //zapis pikseli do vectora
    SDL_Color tempPixel;
    RGBPixel tempOwnPixel;
    for(unsigned int i = 0; i < header.height_f; i++){
        for(unsigned int j = 0; j < header.width_f; j++ ){
            //chce isc od lewej do prawej od gory do dolu
            //przepisuje z sdlcolor na rgbpixel
            tempPixel = getPixel(j,i);
            tempOwnPixel.Red = tempPixel.r;
            tempOwnPixel.Green = tempPixel.g;
            tempOwnPixel.Blue = tempPixel.b;
            uncompressedPixels.push_back(tempOwnPixel);
        }
    }
    header.size_f = sizeof(Header) + sizeof(RGBPixel) * uncompressedPixels.size();
}

RPRP::RPRP(ifstream& rpFile){
    rpFile.read((char*)&header,sizeof(Header)); // odczytuje headera
    unsigned int pixelAmount = header.height_f * header.width_f;
    RGBPixel tempOwnPixel;
    for(unsigned int i = 0; i < pixelAmount; i++){
        rpFile.read((char*)&tempOwnPixel,sizeof(RGBPixel));
        uncompressedPixels.push_back(tempOwnPixel);
    }
    rpFile.close();
}

void RPRP::saveFile(string path){
    ofstream file;
    file.open(path.c_str(), ios::binary | ios::trunc);
    if(!file.good()){
        cout <<"Blad otwarcia pliku do zapisu." << endl;
        exit(1);

    }
    //zapis headera
    file.write((const char*)&header, sizeof(Header));
    //zapis pikseli
    RGBPixel tempOwnPixel;
    for(unsigned int i = 0; i < uncompressedPixels.size(); i++){
        tempOwnPixel = uncompressedPixels[i];
        file.write((const char*)&tempOwnPixel,sizeof(RGBPixel));
    }
    file.close();
    cout <<"Zapisano w RPRP." << endl;
}

SDL_Surface* RPRP::RprpToBmp()
{
    SDL_Surface *bmp = new SDL_Surface;
    bmp->format = new SDL_PixelFormat;
    bmp ->format_version = 2; //daje 2 bylo 1
    bmp->h = header.height_f;
    bmp->w = header.width_f;
    bmp->flags = 0;
    bmp->offset = 0;
    bmp->format->colorkey = 0;
    bmp->format->palette = NULL;

    bmp->format->Rmask = 16711680;
    bmp->format->Gmask = 65280;
    bmp->format->Bmask = 255;
    bmp->format->Amask = 0;
    bmp->format->Rshift = 0; //bylo 16 daje 0
    bmp->format->Gshift = 0; //bylo 8 daje 0
    bmp->format->Bshift = 0;
    bmp->format->Ashift = 0;
    bmp->format->BytesPerPixel = 3;
    bmp->format->BitsPerPixel = 24;
    bmp->format->Rloss = 0;
    bmp->format->Aloss = 0; //bylo 8
    bmp->format->Gloss = 0;
    bmp->format->Bloss = 0;
    bmp->format->alpha = 0xff;

    bmp->pitch = header.width_f * bmp->format->BytesPerPixel;
    if(bmp->pitch % 4 != 0)
        bmp ->pitch += 4 - (bmp->pitch % 4);

    bmp->pixels = new uint8_t [bmp->pitch * header.height_f];
    //zapisuje pixele do surface
    RGBPixel tempPixel;
    for(unsigned int y = 0; y < header.height_f; y++){
        for(unsigned int x = 0; x < header.width_f; x++){
            tempPixel = uncompressedPixels[y * header.width_f + x];
            //zmieniam kolejnosc na b,g,r
            ((Uint8*)bmp->pixels)[ y * bmp->pitch + (3 * x)] = tempPixel.Blue;
            ((Uint8*)bmp->pixels)[ y * bmp->pitch + (3 * x) + 1] = tempPixel.Green;
            ((Uint8*)bmp->pixels)[ y * bmp->pitch + (3 * x) + 2] = tempPixel.Red;
        }
    }
    return bmp;
}



void RPRP::show(){
    cout << "Oznaczenie: " << this->header.type_f[0] << this->header.type_f[1] << endl;
    cout << "Wysokosc: " << this->header.height_f << endl;
    cout << "Szerokosc: " << this->header.width_f << endl;
    cout << "Przesuniecie do danych pikseli: " <<this->header.start_f << endl;
    cout << "Rozmiar pliku: " << this->header.size_f << endl;
    cout << "Wybrana paleta: " << (int)this->header.type_p <<endl;
    //cout << "Ilosc pikseli uncompressed: " << (int)uncompressedPixels.size() << endl;
    int i = uncompressedPixels.size() - 1;
    cout <<"[" << i << "] - " << (int)uncompressedPixels[i].Red << (int)uncompressedPixels[i].Green << (int)uncompressedPixels[i].Blue << endl;

}

void RPRP::saveCompressedFile(string path){
    ofstream file;
    file.open(path.c_str(), ios::binary | ios::trunc);
    if(!file.good()){
        cout <<"Blad otwarcia pliku do zapisu." << endl;
        exit(1);

    }
    //ustaw dobry rozmiar skompresowanego pliku
    this->header.size_f = sizeof(Header) + sizeof(uint8_t) * compressedPixels.size();
    //cout << "Ilosc pikseli uncompressed: " << (int)uncompressedPixels.size() << endl;
    //zapis headera
    file.write((const char*)&header, sizeof(Header));
    //zapis pikseli
    uint8_t temp;
    for(unsigned int i = 0; i < compressedPixels.size(); i++){
        temp = compressedPixels[i];
        file.write((const char*)&temp,sizeof(uint8_t));
    }
    file.close();
    cout <<"Zapisano w RPRP." << endl;
}

RPRP::RPRP(ifstream& rpFile, bool compressed){
    compressed = true;  //nie ma znaczenia
    rpFile.read((char*)&header,sizeof(Header)); // odczytuje headera
    unsigned int numOfReadBytes = header.size_f - sizeof(Header);
    uint8_t temp;

    for(unsigned int i = 0; i < numOfReadBytes; i++){
        rpFile.read((char*)&temp,sizeof(uint8_t));
        compressedPixels.push_back(temp);
    }
    rpFile.close();
}

int power_modulo_fast(int a, int b, int m)
{
	int i;
	int result = 1;
	long int x = a % m;

	for (i = 1; i <= b; i <<= 1)
	{
		x %= m;
		if ((b & i) != 0)
		{
			result *= x;
			result %= m;
		}
		x *= x;
	}

	return result % m;
}
