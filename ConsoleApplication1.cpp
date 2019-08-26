#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>

using namespace std;

#include<windows.h>
#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long

#pragma pack(push)
#pragma pack (2)

/*
struct BITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
};*/

#pragma pack(pop)

void print(BITMAPFILEHEADER h)
{
	cout << "bfType: " << hex << h.bfType << endl;
	cout << "bfSize: " << dec << h.bfSize << endl;
	cout << "bfOffBits: " << dec << h.bfOffBits << endl;
}
/*
struct BITMAPINFOHEADER
{
	DWORD biSize;
	int biWidth;
	int biHeight;

	WORD biPlanes;
	WORD biBitCount;
	WORD biCompression;
	DWORD biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;

	DWORD biClrUUsed;
	DWORD biClrImportant;
};
*/

void print(BITMAPINFOHEADER h)
{
	cout << "biSize: " << dec << h.biSize << endl;
	cout << "biWidth: " << h.biWidth << endl;
	cout << "biHeight: " << h.biHeight << endl;
	cout << "biBitCount: " << h.biBitCount << endl;
	cout << "bitCompression:" << h.biCompression << endl;
	cout << "biSizeImagen: " << h.biSizeImage << endl;
	cout << "biXPelsPerMetter: " << h.biXPelsPerMeter << endl;
	cout << "biYPelsPerMetter: " << h.biYPelsPerMeter << endl;
}

class ImageMatrix
{
public:
	int height;
	int width;
	int rowsize;
	BYTE* term;

	ImageMatrix() { height = 0; width = 0; }

	ImageMatrix(int height, int width)
	{
		this->height = height;
		this->width = width;

		rowsize = (3 * width + 3) / 4 * 4;
		term = new BYTE[height * rowsize];
	}

	bool Load(const char* filename)
	{
		BITMAPFILEHEADER h;
		BITMAPINFOHEADER hInfo;

		ifstream f;

		f.open(filename, ios::binary);
		f.seekg(0, f.end);

		cout << "tamaño? : " << f.tellg() << " bytes \n";
		f.seekg(0, f.beg);
		f.read((char*)& h, sizeof(h));
		f.read((char*)& hInfo, sizeof(hInfo));

		print(h);
		print(hInfo);

		if (h.bfType != 0x4d42
			|| hInfo.biClrUsed != 0
			|| hInfo.biBitCount != 24
			|| hInfo.biCompression != 0
			|| hInfo.biPlanes != 1)
		{
			cout << "error!!\n Imagen Invalida! \n";
			f.close();
			return 0;
		}

		width = hInfo.biWidth;
		height = hInfo.biHeight;
		cout << "leyendo desde " << filename << "....\n";

		*this = ImageMatrix(height, width);
		f.read((char*)term, height * width);
		f.close();

		return 1;
	}

	bool Save(const char* filename)
	{
		BITMAPFILEHEADER h = {
			0x42d42,
			54 + rowsize * height,
			0,
			0,
			54
		};

		BITMAPINFOHEADER hInfo = {
			40, width, height, 1, 24, 0, rowsize * height, 3780,3780, 0,0
		};

		cout << "escribiendo en: " << filename << ".... \n";
		ofstream f;
		f.open(filename, ios::binary);
		f.write((char*)& h, 14);
		f.write((char*)& hInfo, 40);
		f.write((char*)term, rowsize * height);
		f.close();

		return 1;
	}

};

ImageMatrix convert2gray(ImageMatrix m)
{
	ImageMatrix mm = ImageMatrix(m.height, m.width);
	for (int y = 0; y < m.height; y++)
	{
		for (int x = 0; x < m.rowsize; x += 3)
		{
			BYTE B = m.term[y * m.rowsize + x];
			BYTE G = m.term[y * m.rowsize + x + 1];
			BYTE R = m.term[y * m.rowsize + x + 2];

			BYTE gray = (BYTE)(0.2989 * R + 0.5870 * G + 0.1140 * B + 0.5);

			if (y < m.height / 2)
				mm.term[y * m.rowsize + x] = mm.term[y * m.rowsize + x + 1] = mm.term[y * m.rowsize + x + 2] = gray;
			else
			{
				mm.term[y * m.rowsize + x] = B;
				mm.term[y * m.rowsize + x + 1] = G;
				mm.term[y * m.rowsize + x + 2] = R;
			}
		}
	}
	return mm;
}

int main()
{
	cout << sizeof(BITMAPFILEHEADER) << endl;
	cout << sizeof(BITMAPINFOHEADER) << endl;
	ImageMatrix m, mm;
	//char name[] = "A.bmp";
	//char nameS[] = "A_gris.bmp";

	m.Load("A.bmp");
	//m.Save("imagen_copia.bmp");

	mm = convert2gray(m);
	mm.Save("A_gris.bmp");

	system("pause");
	return 0;

}