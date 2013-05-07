#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// .jpeg encoder
#include "jpegenc.h"

// constants specific to 640 x 480 image
#define IMG_WIDTH   (640)   //
#define IMG_HEIGHT  (480)   //
#define OUTPUT_FILENAME "jpeg_header.out"

// output file
FILE * fid = NULL;

// global function for writing output encoded .jpeg data
void write_jpeg(const unsigned char * _buffer,
                const unsigned int    _n)
{
    // write data to output file
    fwrite(_buffer, sizeof(uint8_t), _n, fid);
}

int main (int argc, char *argv[])
{
    // try to open input file for reading
    fid = fopen(OUTPUT_FILENAME, "wb");
    if (!fid) {
        fprintf(stderr, "error: %s, cannot open '%s' for writing\n", argv[0], OUTPUT_FILENAME);
        exit(1);
    }

    // write .jpeg header
    huffman_start(IMG_HEIGHT & -8, IMG_WIDTH & -8);

    // close output file and exit
    fclose(fid);
    printf(".jpeg header written to '%s'\n", OUTPUT_FILENAME);

    // return successfully
    return 0;
}
