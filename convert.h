#ifndef __CONVERT__H__
#define __CONVERT__H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <png.h>


typedef 
struct
{
        uint8_t red;
       	uint8_t green;
        uint8_t blue;
} rgb_t;

int writeImage(const char *fileName, int width, int height, rgb_t *buffer, char *title);

#endif
