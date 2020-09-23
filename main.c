/////////////////////
// MANDELBROT ALGO //
/////////////////////


// INCLUDES //
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Windows.h"
#include <string.h>
#include "cplx.h"


////////////////////////
// PARAM UTILISATEURS //
////////////////////////

// // PARAMETRES FENETRES //
#define FEN_CENTRE_X		-1.76021
#define FEN_CENTRE_Y		0.011915
#define FEN_LARGEUR_X		0.00002
#define FEN_NB_POINTS		1000
#define FEN_RAPPORT_X_Y		1.777777 // 16/9

// PARAMETRES FRACTAL //
#define CONVERG_ITER 5000
#define CONVERG_LIM  4

//////////
//////////
//////////




// Fonction écriture BMP
int WriteBitmapFile(const char *filename, int width, int height, unsigned char *imageData);

// MAIN //
int main()
{
	// --------------- //
	// PARAMETRES CODE //
	// --------------- //
	const char *filename = "image.bmp";
	unsigned char *tableau = NULL;
	double step_x, step_y, X_MAX,X_MIN,Y_MAX,Y_MIN,LARGEUR_X,LARGEUR_Y;
	unsigned int ind,jnd,width,height,conv_step, cmpt, NB_VALUES, NB_VALUES_LARG;
	cplx_t p, CENTRE;
			
	// PARAMETRE UTILSATEUR //
	CENTRE.x = 			FEN_CENTRE_X;
	CENTRE.y = 			FEN_CENTRE_Y;
	LARGEUR_X = 		FEN_LARGEUR_X;
	LARGEUR_Y = 		LARGEUR_X/FEN_RAPPORT_X_Y;
	NB_VALUES_LARG = 	FEN_NB_POINTS;

	// INIT PARAMETRES //
	width = NB_VALUES_LARG; 
	height = (unsigned int) NB_VALUES_LARG * LARGEUR_Y/LARGEUR_X;
	step_x = LARGEUR_X / width;
	step_y = LARGEUR_Y / height;
	NB_VALUES = width * height;
	X_MAX = CENTRE.x + LARGEUR_X/2;
	X_MIN = CENTRE.x - LARGEUR_X/2;
	Y_MAX = CENTRE.y + LARGEUR_Y/2;
	Y_MIN = CENTRE.y - LARGEUR_Y/2;
	
	// ---------------- //
	// CREATION MATRICE //
	// ---------------- //
	tableau = malloc(width * height * sizeof(unsigned char *) );
	
	printf("INIT PARAMETRES : OK\n");
	
	
	cmpt = 0;
	ind=0;
	jnd=0;
	while (jnd<height)
	{
		p.y = Y_MIN + jnd*step_y;
		while (ind<width)
		{
			++cmpt;
			p.x = X_MIN + ind*step_x;
			///
			cplx_t zn, zn1;
			memset(&zn, 0, sizeof(zn));
			for (conv_step = 0; conv_step < CONVERG_ITER; conv_step ++) 
			{
				
				cplx_prod(&zn1, &zn, &zn);
				cplx_add(&zn1, &zn1, &p);

				if (cplx_mod2(&zn1) > CONVERG_LIM) 
				{
				  tableau[cmpt-1] = 255*conv_step/CONVERG_ITER;
				  ++cmpt;
				  tableau[cmpt-1] = 255*conv_step/CONVERG_ITER;
				  ++cmpt;
				  tableau[cmpt-1] = 255*conv_step/CONVERG_ITER;
				  break;
				}

				zn = zn1;
			}

			//
			if (conv_step == CONVERG_ITER)
			{
				tableau[cmpt-1] = 0;
				++cmpt;
				tableau[cmpt-1] = 0;
				++cmpt;
				tableau[cmpt-1] = 0;
			}
			///
			
			p.x += step_x;
			ind++;
		}
		ind=0;
		p.y += step_y;
		jnd++;
	}
	
	
	printf("MATRICE MANDELBROT : OK\n");
	printf("NB_PIXELS = %d\n",NB_VALUES);
	
	
	// -------------------- //
	// CREATION FICHIER BMP //
	// -------------------- //
	if (!WriteBitmapFile(filename,width,height,tableau))
	{
		printf("%s : KO",filename);
	}
	else
	{
		printf("%s : OK",filename);
	}

	
	return EXIT_SUCCESS;
}

// DECLARATION STRUCTURE BMP //
struct BITMAPFILEHEADER
{
	WORD bfType; //specifies the file type
	DWORD bfSize; //specifies the size in bytes of the bitmap file
	WORD bfReserved1; //reserved; must be 0
	WORD bfReserved2; //reserved; must be 0
	DWORD bOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct BITMAPINFOHEADER
{
	DWORD biSize; //specifies the number of bytes required by the struct
	LONG biWidth; //specifies width in pixels
	LONG biHeight; //species height in pixels
	WORD biPlanes; //specifies the number of color planes, must be 1
	WORD biBitCount; //specifies the number of bit per pixel
	DWORD biCompression;//spcifies the type of compression
	DWORD biSizeImage; //size of image in bytes
	LONG biXPelsPerMeter; //number of pixels per meter in x axis
	LONG biYPelsPerMeter; //number of pixels per meter in y axis
	DWORD biClrUsed; //number of colors used by th ebitmap
	DWORD biClrImportant; //number of colors that are important
};

int WriteBitmapFile(const char *filename, int width, int height, unsigned char *imageData)
{
	FILE             *filePtr;        // file pointer
	BITMAPFILEHEADER bitmapFileHeader;    // bitmap file header
	BITMAPINFOHEADER bitmapInfoHeader;    // bitmap info header

	// open file for writing binary mode
	filePtr = fopen(filename, "wb");
	if (!filePtr)
			return 0;

	// define the bitmap file header
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
	bitmapFileHeader.bfType = 0x4D42;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// define the bitmap information header
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;                        // 24-bit
	bitmapInfoHeader.biCompression = BI_RGB;                // no compression
	bitmapInfoHeader.biSizeImage = width * height * sizeof(unsigned char *);    // width * height * (RGB bytes)
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;
	bitmapInfoHeader.biWidth = width;                        // bitmap width
	bitmapInfoHeader.biHeight = height;                    // bitmap height

	// write the bitmap file header
	fwrite(&bitmapFileHeader, 1, sizeof(BITMAPFILEHEADER), filePtr);

	// write the bitmap info header
	fwrite(&bitmapInfoHeader, 1, sizeof(BITMAPINFOHEADER), filePtr);

	// write the image data
	fwrite(imageData, 1, bitmapInfoHeader.biSizeImage, filePtr);

	// close our file
	fclose(filePtr);

	// Success
	return 1;
}

