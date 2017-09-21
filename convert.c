#include "convert.h"

int writeImage(const char *fileName, int width, int height, rgb_t *buffer, char *title)
{
	printf("Saving PNG\n");

	int err = 0;
	if (fileName == NULL)
	{
		err = 1;
		goto finalise;
	}

	FILE *fp = NULL;
	
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	
	// Open file for writing (binary mode)
	fp = fopen(fileName, "wb+");
	if (fp == NULL) 
	{
		fprintf(stderr, "Could not open file %s for writing \n", fileName);
		err = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) 
	{
		fprintf(stderr, "Could not allocate write struct\n");
		err = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) 
	{
		fprintf(stderr, "Could not allocate info struct\n");
		err = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) 
	{
		fprintf(stderr, "Error during png creation\n");
		err = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	// TODO DEPTH
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) 
	{
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);
	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y = 0 ; y < height ; y++) 
	{
		for (x = 0 ; x < width ; x++) 
		{
			row[x * 3]     = buffer[y * width + x].red;
			row[x * 3 + 1] = buffer[y * width + x].green;
			row[x * 3 + 2] = buffer[y * width + x].blue;
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

	finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return err;
}
