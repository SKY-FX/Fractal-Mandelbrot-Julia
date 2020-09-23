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

// PARAMETRES FENETRES //
#define FEN_CENTRE_X		-0.5//-1.76021079962025
#define FEN_CENTRE_Y		0//0.0119148299634
#define FEN_LARGEUR_X		4//0.000000000002
#define FEN_NB_POINTS		512
#define FEN_RAPPORT_X_Y		1 // 16/9

// PARAMETRE AFFICHAGE
#define MULTIPLI 			50
//#define ZOOM				2

// PARAMETRES FRACTAL //
#define CONVERG_ITER 500//350000
#define CONVERG_LIM  4

//////////
//////////
//////////


// Fonction écriture BMP
int WriteBitmapFile(const char *filename, int width, int height, unsigned char *imageData);
int FFT2D(cplx_t **c,int nx,int ny,int dir);
int Powerof2(int n,int *m,int *twopm);
int FFT(int dir,int m,double *x,double *y);


// MAIN //
int main()
{
	// --------------- //
	// PARAMETRES CODE //
	// --------------- //
	const char *filename = "IN_Mandel.bmp";
	const char *filenameFFT = "OUT_FFT.bmp";
	unsigned char **tab_FFT_tmp, *tab_BMP = NULL;
	cplx_t **tab_FFT;
	//unsigned char *tableauFFT = NULL;
	double step_x, step_y,X_MIN,Y_MIN,LARGEUR_X,LARGEUR_Y;
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
	//X_MAX = CENTRE.x + LARGEUR_X/2;
	X_MIN = CENTRE.x - LARGEUR_X/2;
	//Y_MAX = CENTRE.y + LARGEUR_Y/2;
	Y_MIN = CENTRE.y - LARGEUR_Y/2;
	
	
	
	// ---------------- //
	// CREATION MATRICE //
	// ---------------- //
	tab_BMP 		= (unsigned char*)calloc(width * height, sizeof(unsigned char*) );
	tab_FFT 		= (cplx_t **)malloc(height * sizeof(cplx_t *));
	tab_FFT_tmp 	= (unsigned char **)malloc(height * sizeof(unsigned char*));
	//
	cmpt = 0;
	if(tab_FFT != NULL)
	{
		for (cmpt = 0; cmpt < height; cmpt++)
		{
			tab_FFT[cmpt] = calloc(width, sizeof(cplx_t));
		}
	} else printf("FAKE CREATION TAB_FFT");
	//
	cmpt = 0;
	if(tab_FFT_tmp != NULL)
	{
		for (cmpt = 0; cmpt < height; cmpt++)
		{
			tab_FFT_tmp[cmpt] = calloc(width, sizeof(unsigned char));
		}
	} else printf("FAKE CREATION TAB_FFT_TMP");
	
	
	printf("\nINIT PARAMETRES : OK\n");
	printf("MATRICE MANDELBROT...");
	
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
				  tab_BMP[cmpt-1] = 255*conv_step/CONVERG_ITER;
				  ++cmpt;
				  tab_BMP[cmpt-1] = 255*conv_step/CONVERG_ITER;
				  ++cmpt;
				  tab_BMP[cmpt-1] = 255*conv_step/CONVERG_ITER;
				  break;
				}

				zn = zn1;
			}

			//
			if (conv_step == CONVERG_ITER)
			{
				tab_BMP[cmpt-1] = 255;
				++cmpt;
				tab_BMP[cmpt-1] = 255;
				++cmpt;
				tab_BMP[cmpt-1] = 255;
			}
			///
			
			p.x += step_x;
			ind++;
		}
		ind=0;
		p.y += step_y;
		jnd++;
	}
	printf(" : OK\n");
	
	// -------------------- //
	// CREATION FICHIER BMP //
	// -------------------- //
	if (!WriteBitmapFile(filename,width,height,tab_BMP))
	{
		printf("%s : KO",filename);
	}
	else
	{
		printf("%s : OK",filename);
	}
	printf("\nNB_PIXELS = %d\n\n",NB_VALUES);
	
	
	
	// ---------- //
	// CALCUL FFT //
	// ---------- //
	//Construction matrice 2D pour la FFT
	int nIndice = 0;
	int resultFFT = 0;
	for(ind=0; ind<height; ind++)
	{
		for(jnd=0; jnd<width; jnd++)
		{
			nIndice = ind*width + jnd;
			tab_FFT[ind][jnd].x = tab_BMP[3*nIndice];
			tab_FFT[ind][jnd].y = 0;
		}
	}
	
	printf("CALCUL FFT...");
	//	
	resultFFT = FFT2D(tab_FFT, height, width, 1);
	if (resultFFT==0) printf("\nFAKE FFT2D function\n");
	else printf(" : OK\n");
	printf("Creation image...\n");
	

	//Calcul du Module + Normalisation
	double Module = 0.0;
	double maxModule = 0.0;
	nIndice = 0;
	for(ind=0; ind<height; ind++)
	{
		for(jnd=0; jnd<width; jnd++)
		{
			//Cherche le max
			Module = sqrt((tab_FFT[ind][jnd].x)*(tab_FFT[ind][jnd].x) + (tab_FFT[ind][jnd].y)*(tab_FFT[ind][jnd].y));
			if (Module > maxModule) maxModule = Module;
			
			// Normalise
			Module = MULTIPLI * 255 * (1-Module/maxModule);
			// if (Module>255) Module = 255;
			
			// Init
			tab_FFT[ind][jnd].x = Module;
			tab_FFT[ind][jnd].y = 0;	
		}
	}
	
	
	// -------------------------------------------- //
	// Modification image BMP FFT AVEC DC AU CENTRE //
	// -------------------------------------------- //	
	unsigned int L1 = height;
	unsigned int L2 = height/2;
	for (ind=0;ind<L2;ind++)
	{
		for (jnd=0;jnd<L2;jnd++)
		{			
			tab_FFT_tmp[L2 - ind - 1][L2 - jnd - 1] = tab_FFT[ind][jnd].x;
			tab_FFT_tmp[L2 - ind - 1][L1 - jnd - 1] = tab_FFT[ind][L2 + jnd].x;
			tab_FFT_tmp[L1 - ind - 1][L2 - jnd - 1] = tab_FFT[L2 + ind][jnd].x;
			tab_FFT_tmp[L1 - ind - 1][L1 - jnd - 1] = tab_FFT[L2 + ind][L2 + jnd].x;
		}
	}
	
	// ------------------------ //
	// CREATION FICHIER FFT_BMP //
	// ------------------------ //
	nIndice = 0;
	// Creation matrice dortie
	for (ind=0;ind<height;ind++)
	{
		for (jnd=0;jnd<width;jnd++)
		{						
			// Rempli pour la sortie BMP RVB
			tab_BMP[nIndice] = (unsigned char)tab_FFT_tmp[ind][jnd];
			++nIndice;
			tab_BMP[nIndice] = (unsigned char)tab_FFT_tmp[ind][jnd];
			++nIndice;
			tab_BMP[nIndice] = (unsigned char)tab_FFT_tmp[ind][jnd];
			++nIndice;	
			
		}
	}	
	// Creation du fichier bmp
	if (!WriteBitmapFile(filenameFFT,L1,L1,tab_BMP))
	{
		printf("%s : KO",filenameFFT);
	}
	else
	{
		printf("%s : OK",filenameFFT);
	}
	

	// fclose(filename);
	// fclose(filenameFFT);
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

/*-------------------------------------------------------------------------
   Perform a 2D FFT inplace given a complex 2D array
   The direction dir, 1 for forward, -1 for reverse
   The size of the array (nx,ny)
   Return false if there are memory problems or
      the dimensions are not powers of 2
*/
int FFT2D(cplx_t **c,int nx,int ny,int dir)
{
   int i,j;
   int m,twopm;
   double *real,*imag;

   /* Transform the rows */
   real = (double *)malloc(nx * sizeof(double));
   imag = (double *)malloc(nx * sizeof(double));
   if (real == NULL || imag == NULL)
      return(FALSE);
   if (!Powerof2(nx,&m,&twopm) || twopm != nx)
      return(FALSE);
   for (j=0;j<ny;j++) {
      for (i=0;i<nx;i++) {
         real[i] = c[i][j].x;
         imag[i] = c[i][j].y;
      }
      FFT(dir,m,real,imag);
      for (i=0;i<nx;i++) {
         c[i][j].x = real[i];
         c[i][j].y = imag[i];
      }
   }
   free(real);
   free(imag);

   /* Transform the columns */
   real = (double *)malloc(ny * sizeof(double));
   imag = (double *)malloc(ny * sizeof(double));
   if (real == NULL || imag == NULL)
      return(FALSE);
   if (!Powerof2(ny,&m,&twopm) || twopm != ny)
      return(FALSE);
   for (i=0;i<nx;i++) {
      for (j=0;j<ny;j++) {
         real[j] = c[i][j].x;
         imag[j] = c[i][j].y;
      }
      FFT(dir,m,real,imag);
      for (j=0;j<ny;j++) {
         c[i][j].x = real[j];
         c[i][j].y = imag[j];
      }
   }
   free(real);
   free(imag);

   return(TRUE);
}

/*-------------------------------------------------------------------------
   This computes an in-place complex-to-complex FFT
   x and y are the real and imaginary arrays of 2^m points.
   dir =  1 gives forward transform
   dir = -1 gives reverse transform

     Formula: forward
                  N-1
                  ---
              1   \          - j k 2 pi n / N
      X(n) = ---   >   x(k) e                    = forward transform
              N   /                                n=0..N-1
                  ---
                  k=0

      Formula: reverse
                  N-1
                  ---
                  \          j k 2 pi n / N
      X(n) =       >   x(k) e                    = forward transform
                  /                                n=0..N-1
                  ---
                  k=0
*/
int FFT(int dir,int m,double *x,double *y)
{
   long nn,i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
   nn = 1;
   for (i=0;i<m;i++)
      nn *= 2;

   /* Do the bit reversal */
   i2 = nn >> 1;
   j = 0;
   for (i=0;i<nn-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0;
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0;
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<nn;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1;
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1)
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      for (i=0;i<nn;i++) {
         x[i] /= (double)nn;
         y[i] /= (double)nn;
      }
   }

   return(TRUE);
}

/*-------------------------------------------------------------------------
   Calculate the closest but lower power of two of a number
   twopm = 2**m <= n
   Return TRUE if 2**m == n
*/
int Powerof2(int n,int *m,int *twopm)
{
   if (n <= 1) {
      *m = 0;
      *twopm = 1;
      return(FALSE);
   }

   *m = 1;
   *twopm = 2;
   do {
      (*m)++;
      (*twopm) *= 2;
   } while (2*(*twopm) <= n);

   if (*twopm != n)
      return(FALSE);
   else
      return(TRUE);
}