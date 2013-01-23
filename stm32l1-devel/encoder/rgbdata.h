/*
 * rgbdata.h
 *
 * This header provides functionality for jpegant to access RGB pixels from raw
 * RGB data.
 * On the x86 platform we do this with a memory-mapped file
 *
 * For the ARM platform, the functions request data from SPI Flash
 *
 * This functionality is **very specific** to a 640x480 pixel image, as this 
 * code is purpose built for AMSAT FOX-1
 *
 * Mitch Davis
 * Virginia Tech, 2012
 * mitchd@vt.edu
 */


#include <stdint.h>

/*
 * Define our image sizes here
 */
#define IMG_WIDTH 640 
#define IMG_HEIGHT 480

typedef uint8_t color;


typedef struct {
        color A;
        color B;
} RGB;


