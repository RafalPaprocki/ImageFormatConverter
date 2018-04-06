#include "FormatBitConverter.h"

using namespace std;

#undef main //uzywam zeby informacje byly wypisywane do konsoli

SDL_Surface* BmpSurface = NULL;

string saveFileName;
clock_t clockStart, clockEnd;
ifstream rpFile;

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
		cout << "Nacisnij 'q' lub 'Q' aby wyjsc" << endl;
        cin >> command;
        fflush(stdin);
        switch (command)
        {
            case '1':
            {
                  loadBmp();

                  while( paletteType < 0 || paletteType > 2 )
				  {
                      cout << "Wybierz rodzaj palety w ktorej obraz ma byc zapisany:" << endl;
                      cout <<"[0] Zwykla kolorowa paleta" << endl;
                      cout <<"[1] Paleta dopasowana (MedianCut)" << endl;
                      cout <<"[2] Skala szarosci" << endl;
                      cin >> paletteType;
                      fflush(stdin);
                    
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
                while( paletteType < 0 || paletteType > 2 )
				{
                    cout << "Wybierz rodzaj palety w ktorej obraz ma byc zapisany:" << endl;
                    cout <<"0. Zwykla kolorowa paleta" << endl;
                    cout <<"1. Paleta dopasowana" << endl;
                    cout <<"2. Skala szarosci" << endl;
                    cin >> paletteType;
                    fflush(stdin);
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
                Rprp.preparePixelsForCompression();

                fSizeUncomp = sizeof(RPRP::Header) + Rprp.temporaryUncompressedPixels.size() * sizeof(uint8_t);

                cout <<"Rozpoczynam kompresje." << endl;
                clockStart = clock();

                Rprp.LZ77_Compression(Rprp.temporaryUncompressedPixels, Rprp.compressedPixels);
                clockEnd = clock();

                cout <<"Koncze kompresje." << endl;
                cout <<"Kompresja trwala: " <<((double)(clockEnd - clockStart)) / (double)CLOCKS_PER_SEC << " sekund" << endl;
                fSizeComp = sizeof(RPRP::Header) + Rprp.compressedPixels.size() * sizeof(uint8_t);
                cout <<"Rozmiar pliku po kompresji wynosi: " << fSizeComp << " ,a to jest " << ((double)fSizeComp / (double)fSizeUncomp) * 100 << "% oryginalnego rozmiaru pliku" <<  endl;

                Rprp.saveCompressedFile(saveFileName.c_str());
                cout <<"Pomyslnie zapisano plik .rprp" << endl << endl;
                break;

                break;
            }
            case '3':
            {
                openRprp();
         
                RPRP Rprp(rpFile,true);
                Rprp.LZ77_Decompression(Rprp.temporaryUncompressedPixels, Rprp.compressedPixels, Rprp.header.width_f * Rprp.header.height_f * 3);
                Rprp.preparePixelsForSavingBMP();
                
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


