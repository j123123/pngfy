#ifndef PNGRW_H
#define PNGRW_H
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <setjmp.h>
int read_png (const char *filename, char **data, unsigned int *width, unsigned int *height);
int write_png(const char *file_name,const unsigned char *rgba,int width,int height);
#endif
