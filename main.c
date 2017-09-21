#include <stdio.h>
#include "convert.h"

struct globalArgs_t
{
	const char *outFileName; // -o parametr
	FILE *outFile;
	int depth;
	char **inputFiles;
	int numInputFiles;
} globalArgs;

static const char *optString = "o:h?";

void usage(const char *programName)
{
	printf("Usage [OPTIONS: -o -h ?] inputFiles");

	exit( EXIT_FAILURE );
}

uint8_t* readPxlBufferFromFile(const char *fileName, size_t *fSize)
{
	FILE *fp = NULL;
	
	fp = fopen(fileName, "rb");
	if (fp == NULL) 
	{
		fprintf(stderr, "Could not open file %s for reading Pixel Buffer", fileName);
		exit( EXIT_FAILURE );
	}

	fseek(fp, 0, SEEK_END);
	*fSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t *pxlBuf = (uint8_t*) malloc(sizeof(uint8_t) * (*fSize));
	
	if (fread(pxlBuf, sizeof(uint8_t), (size_t) *fSize, fp) != *fSize)
	{
		fprintf(stderr, "Error reading Pixel buffer from file %s", fileName); 
		
		exit( EXIT_FAILURE );
	}

	fclose(fp);
	return pxlBuf;
}

rgb_t YUV_to_RGB(int y, int u, int v)
{
	int r, g, b;
	rgb_t rgb;

	// u and v are +-0.5
	u -= 128;
   	v -= 128;

   	// Conversion
   	r = y + 1.370705 * v;
   	g = y - 0.698001 * v - 0.337633 * u;
   	b = y + 1.732446 * u;
/*
   r = y + 1.402 * v;
   g = y - 0.344 * u - 0.714 * v;
   b = y + 1.772 * u;*/

	if (r < 0)   r = 0;
        if (g < 0)   g = 0;
        if (b < 0)   b = 0;
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;

	rgb.red   = r;
	rgb.green = g;
	rgb.blue  = b;

	return rgb;
}

rgb_t* convert422(uint8_t *yuv, size_t bufSize)
{
	// yuv 4:2:2 format - (4 bytes per 2 pixel) bit depth 16 bit 

	//TODO CONST & (each (2 pixels = 4 bytes) = 6 bytes (rgb))
	rgb_t* buf = (rgb_t*) malloc(sizeof(rgb_t) * (bufSize * 3 / 2));

	if (buf == NULL)
	{
		fprintf(stderr, "Error in allocating memory for rgb buffer\n");
		exit( EXIT_FAILURE );
	}

	int pos = 0;
	int ind = 0;
	for (pos = 0; pos < bufSize; pos += 4)
	{
		int y1, y2, u, v;

		// Extract yuv components
	
		y1 = yuv[pos];
		u  = yuv[pos + 1];
		y2 = yuv[pos + 2];
		v  = yuv[pos + 3];
	
	/*
		u = yuv[pos];
		y1  = yuv[pos + 1];
		v = yuv[pos + 2];
		y2  = yuv[pos + 3];
	*/
		buf[ind++] = YUV_to_RGB(y1, u, v);
		buf[ind++] = YUV_to_RGB(y2, u, v);
	}

	return buf;
}


void dump(uint8_t* buf, size_t size)
{
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("[DUMP] \n");
	printf("buffer address : %p | buf size : %d\n", buf, size);

	int i;
	for (i = 0; i < size; i++)
	{
		if (i % 25 == 0) printf("\n");
		printf("%x   ", buf[i]);

	} 

	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("[END] \n");
}

int main(int argc, char **argv)
{
	int opt = 0;

	globalArgs.outFileName = NULL;
	globalArgs.inputFiles  = NULL;
	globalArgs.numInputFiles = 0;
	opt = getopt( argc, argv, optString);

	while ( opt != -1 ) 
	{
		switch ( opt ) 
		{
			case 'o':
				globalArgs.outFileName = optarg;
				break;
			case 'h':
					case '?':
				usage(argv[0]);
				break;
		}
	
		opt = getopt( argc, argv, optString);
	}

	globalArgs.inputFiles = argv + optind;
	globalArgs.numInputFiles = argc - optind;

	size_t bufSize;
	uint8_t* yuv = readPxlBufferFromFile(*globalArgs.inputFiles, &bufSize);
	rgb_t*   rgb = convert422(yuv, bufSize);
	int code = writeImage(globalArgs.outFileName, 176, 144, rgb, "My first PNG");
	
//	dump(yuv, bufSize);

	if (code != 0)
	{
		fprintf(stderr, "Occur error in writing PNG");
		exit( EXIT_FAILURE );
	}
	
	free(yuv);
	free(rgb);

	return 0;
}
