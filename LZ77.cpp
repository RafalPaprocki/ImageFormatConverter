#include "RPRP.h"

using namespace std;



const int slo=250; 	//dlugosc bufora slownikowego 16 384
const int buf=250; 	//dlugosc bufora wejsciowego 258
int PCS[3];	//tablica przechowujaca P,C,S

void RPRP::preparePixelsForCompression(){
    for(unsigned int i = 0; i < this->uncompressedPixels.size(); i++){
        this->temporaryUncompressedPixels.push_back(this->uncompressedPixels[i].Red);
        this->temporaryUncompressedPixels.push_back(this->uncompressedPixels[i].Green);
        this->temporaryUncompressedPixels.push_back(this->uncompressedPixels[i].Blue);
    }
}

void RPRP::preparePixelsForSavingBMP(){
    RGBPixel tempPixel;
    for(unsigned int i = 0; i < temporaryUncompressedPixels.size(); i+=3){

        tempPixel.Red = temporaryUncompressedPixels[i];
        tempPixel.Green = temporaryUncompressedPixels[i + 1];
        tempPixel.Blue = temporaryUncompressedPixels[i + 2];

        uncompressedPixels.push_back(tempPixel);

    }
}



void RPRP::LZ77_Compression(vector<uint8_t> code, vector<uint8_t> & result)
{
	const int r = 256;   
	const int q = 9551;  
	uint8_t tekst[32];
	uint8_t wzorzec[32];
	uint8_t position, length;
	int m, n, i, j, h1, h2, rm, l;

	for (int z = 0; z < 32; z++)
	{
		tekst[z] = code[0];
	}
	result.push_back(code[0]);
	while (!code.empty())
	{

		length = 0; position = 0;
		l = 1;
		while ((l < 32) && (l < code.size()))
		{
			int k = 0;
			while (k < l)
			{
				wzorzec[k] = code[k];
				k++;
			}

			n = 32;
			m = k;
			h2 = 0;
			h1 = 0;


			for (i = 0; i<m; i++)
			{
				h2 = ((h2*r) + tekst[i]);
				h2 %= q;
			}

			for (i = 0; i<m; i++)
			{
				h1 = ((h1*r) + wzorzec[i]);
				h1 %= q;
			}

			rm = power_modulo_fast(r, m - 1, q);
			i = 0;
			while (i<n - m)
			{
				j = 0;
				if (h1 == h2) while ((j<m) && (wzorzec[j] == tekst[i + j])) j++;
				if (j == m)
				{
					if (l>length)
					{
						position = i;
						length = l;
					}
				}
				h2 = ((h2 - tekst[i] * rm)*r + tekst[i + m]);
				h2 %= q;
				if (h2<0) h2 += q;
				i++;
			}
			j = 0;
			if (h1 == h2) while ((j<m) && (wzorzec[j] == tekst[i + j])) j++;
			if (j == m)
			{
				if (l>length)
				{
					position = i;
					length = l;
				}
			}
			l++;
		}

		result.push_back(code[length]);
		result.push_back(position);
		result.push_back(length);

		j = 0;
		for (i = length + 1; i < 32; i++)
		{
			tekst[j] = tekst[i];
			j++;
		}

		i = 0;
		while (j < 32)
		{
			tekst[j] = code[i];
			j++; i++;
		}

		code.erase(code.begin(), code.begin() + length + 1);
	}
}

void RPRP::LZ77_Decompression(vector<uint8_t> & code, std::vector<uint8_t> & dane, int codesize)
{
	int data, i, j;
	uint8_t tekst[32], position, length;

	int ppose = 0;
	data = dane[0];

	for (int z = 0; z < 32; z++)
	{
		tekst[z] = data;
	}

	int k = 1;
	while (ppose < codesize)
	{
		data = dane[k];
		position = dane[k + 1];
		length = dane[k + 2];

		j = position;
		for (i = 0; i < length; i++)
		{
			code.push_back(tekst[j]);
			j++;
		}
		code.push_back(data);

		j = 0;
		for (i = length + 1; i < 32; i++)
		{
			tekst[j] = tekst[i];
			j++;
		}

		for (i = ppose; i < (ppose + length + 1); i++)
		{
			tekst[j] = code[i];
			j++;
		}
		ppose = ppose + length + 1;
		k += 3;
	}
}



int RPRP::fh(int dl,int indeks, uint8_t tab[])	
{
	int h=0;
	for(int i=indeks;i<dl+indeks;i++)
	{
		  h = 2 * h + 1 - (tab[i] & 1);
	}
	return h %256;
}

int RPRP::sprawdzenie(int i,int dl,int indeks_b, uint8_t tab[])
{
	int j=i;
	int pom=0;	
	for(j;j<(i+dl);j++)
	{
		if(tab[indeks_b]==tab[j])
			pom++;
		indeks_b++;
	}
	return pom;
}

bool RPRP::WyszukiwanieWzorca(int dl,int indeks_b,int indeks_s, uint8_t tab[])
{
	int spr;
	int i=indeks_s; 
	int h_slownika=fh(dl,i,tab);
	int h_bufora=fh(dl,indeks_b,tab);
	int pom=0;
	while(true)
	{
		if(h_slownika==h_bufora) 
		{
			spr=sprawdzenie(i,dl,indeks_b,tab);
			if(dl==spr)
			{
				PCS[0]=pom; 
				PCS[1]=dl;	
				PCS[2]=tab[indeks_b+dl];	
				return true;
			}

		}
		i++;
		pom++;
		if(i==indeks_b-dl) break;	
		h_slownika=fh(dl,i,tab);
	}
	return false;

}

void RPRP::LZ77(vector<uint8_t> dane,int R, vector<uint8_t> &kompresja)
{   
    uint8_t *tab = new uint8_t[R+slo];	

	for(int i=0;i<R;i++)
    {
		tab[i+slo]=dane[i];	//przepisanie danych do tablicy
    }
	for(int i=0;i<slo;i++)	//uzupelnienie slownika(tablicy od indeksu 0 do slo) pierwszym elementem bufora
		tab[i]=tab[slo];
	kompresja.push_back(tab[0]);
	int indeks_b=slo;	//indeks bufora,zaczyna sie za slownikiem
	int indeks_s=0;		//indeks slownika
	while(indeks_b<=R-buf)
	{

		PCS[0]=0; //gdy nie istnieje wspolny podciag
		PCS[1]=0;
		PCS[2]=tab[indeks_b];
		int dl=buf-1;	//dl = dlugosci bufora pomniejszona o jeden
		while(dl>0)
		{
			if(WyszukiwanieWzorca(dl,indeks_b,indeks_s,tab)==true)	//sprawdzenie czy istnieje podciag w slowniku(na poczatku o dlugosci bufora, nastepnie zmniejszany)
			{
				break;
			}
			dl--;
		}

		for(int i=0;i<3;i++)
        {
			kompresja.push_back(PCS[i]);
        }
		indeks_b+=(PCS[1]+1);	//okno bufora przesuwamy o C+1
		indeks_s+=(PCS[1]+1);

	}
	while(indeks_b<R)
	{
		PCS[0]=0; //gdy nie istnieje wspolny podciag
		PCS[1]=0;
		PCS[2]=tab[indeks_b];
		int dl=R-indeks_b-1;	//dl = dlugosci bufora pomniejszona o jeden ale nie za dluga aby nie wyszla za tablice
		while(dl>0)
		{

			if(WyszukiwanieWzorca(dl,indeks_b,indeks_s,tab)==true)	//sprawdzenie czy istnieje podciag w slowniku(na poczatku o dlugosci bufora, nastepnie zmniejszany)
			{
				break;
			}

			dl--;
		}
		for(int i=0;i<3;i++)
        {
            kompresja.push_back(PCS[i]);
        }

		indeks_b+=(PCS[1]+1);	//okno bufora przesuwamy o C+1
		indeks_s+=(PCS[1]+1);
	}
	//cout << "Zakonczylem kompresje" << endl;
}

void RPRP::LZ77Dekompresja( vector<uint8_t> kompresja,  vector<uint8_t> &dekompresja)
{
    vector<uint8_t> slownik;
	for(int i=0;i<slo;i++)
    {//wypelnienie slownika pierwszym elementem
		slownik.push_back(kompresja[0]);

    }
	int ile=0;	//wielkosc tablicy dekompresja
	int n;
	int limit=kompresja.size()-2;
	for(int i=1;i<limit;i+=3)	//spisanie wszystkich danych z kompresji
	{
		for(int j=kompresja[i];j<(kompresja[i+1]+kompresja[i]);j++)	//wpisanie do dekompresji odpowiedniej ilosci wyrazow ze slownika od P do C
		{
			if(j>=slo)
			{
				dekompresja.push_back(dekompresja[ile-1]);
				ile++;
			}
			else
			{
				dekompresja.push_back(slownik[j]);
				ile++;
			}
		}
		n=ile-kompresja[i+1];
		for(int j=0;j<kompresja[i+1];j++)	//zaktualizowanie slownika
		{

			slownik.erase(slownik.begin());
			slownik.push_back(dekompresja[n]);
			n++;
		}
		dekompresja.push_back(kompresja[i+2]);	//dopisanie elementu S
		ile++;
		slownik.erase(slownik.begin());
		slownik.push_back(kompresja[i+2]);
	}
}

