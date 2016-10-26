/* marker */
#ifndef PVRLIB_H
#define PVRLIB_H



/* dependencies */
#include <stdio.h>
#include <memory.h>


/* c++ marker */
#ifdef __cplusplus
extern "C"
{
#endif

int PVRDecompressRGB_2bpp(unsigned char *src, int width, int height, unsigned char *dest );
int PVRDecompressRGBA_2bpp(unsigned char *src, int width, int height, unsigned char *dest );
int PVRDecompressRGB_4bpp(unsigned char *src, int width, int height, unsigned char *dest );
int PVRDecompressRGBA_4bpp(unsigned char *src, int width, int height, unsigned char *dest );

int PVRCompressA8R8G8B8ToRGB4bpp(unsigned char* src, int width, int height, unsigned char* dest);


/* end marker */
#ifdef __cplusplus
}
#endif

#endif