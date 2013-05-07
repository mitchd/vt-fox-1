#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// .jpeg encoder
#include "jpegenc.h"

// constants specific to 640 x 480 image
#define IMG_WIDTH   (640)   //
#define IMG_HEIGHT  (480)   //
#define NUM_LINES   (60)    // number of lines in image

// output file
FILE * file_jpg = NULL;

// global function for writing output encoded .jpeg data
void write_jpeg(const unsigned char * _buffer,
                const unsigned int    _n)
{
    // write data to output file
    //write(file_jpg, _buffer, _n);
    fwrite(_buffer, sizeof(uint8_t), _n, file_jpg);
}

int main (int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s file-in.[rgb|rgb16|yuv] file-out.jpg\n", argv[0]);
        return -1;
    }

    // image type
    enum {
        TYPE_UNKNOWN=0,
        TYPE_RGB24,
        TYPE_RGB16,
        TYPE_YUV,
    } image_type = TYPE_UNKNOWN;
    unsigned int line_size = 0;

    // get image type from filename extension
    char * ext = strpbrk(argv[1],".");
    if      (ext == NULL            ) image_type = TYPE_UNKNOWN;
    else if (strcmp(ext,".rgb24")==0) image_type = TYPE_RGB24;
    else if (strcmp(ext,".rgb16")==0) image_type = TYPE_RGB16;
    else if (strcmp(ext,".yuv")  ==0) image_type = TYPE_YUV;
    else                              image_type = TYPE_UNKNOWN;

    switch (image_type) {
    case TYPE_RGB24:    line_size = 15360;  break;
    case TYPE_RGB16:    line_size = 10240;  break;
    case TYPE_YUV:      line_size = 10240;  break;
    case TYPE_UNKNOWN:
    default:
        fprintf(stderr, "error: %s, unknown file type for input '%s'\n", argv[0], argv[1]);
        exit(1);
    }

    // try to open input file for reading
    FILE * file_rgb = fopen(argv[1], "rb");
    if (!file_rgb) {
        fprintf(stderr, "error: %s, cannot open '%s' for reading\n", argv[0], argv[1]);
        exit(1);
    }

    // try to open output file for writing
    file_jpg = fopen(argv[2], "wb");
    if (!file_rgb) {
        fprintf(stderr, "error: %s, cannot open '%s' for writing\n", argv[0], argv[2]);
        fclose(file_rgb);
        exit(1);
    }

    // line buffer
    uint8_t line_buffer[line_size];

    // write .jpeg header and start-of-image marker
    huffman_start(IMG_HEIGHT & -8, IMG_WIDTH & -8);
    huffman_resetdc();

    // try to load data from file one line at a time
    // number of lines in image (specific to 640 x 480 image and encoding rate)
    unsigned int line;
    for (line=0; line<NUM_LINES; line++) {
        // load line from input
        int num_read = fread(line_buffer, sizeof(uint8_t), line_size, file_rgb);

        // ensure data are read properly
        // TODO: check for premature end of file
        if (num_read != line_size) {
            fprintf(stderr,"error: %s, could not read data from input file (got %d bytes, expected %u)\n",
                    argv[0], num_read, line_size);
            fclose(file_rgb);
            fclose(file_jpg);
            exit(1);
        }

        // encode the line using appropriate encoder
        switch (image_type) {
        case TYPE_RGB24:    encode_line_rgb24(line_buffer, line); break;
        case TYPE_RGB16:    encode_line_rgb16(line_buffer, line); break;
        case TYPE_YUV:      encode_line_yuv(line_buffer,   line); break;
        case TYPE_UNKNOWN:
        default:
            fprintf(stderr, "error: %s, unsupported encoder for input '%s'\n", argv[0], argv[1]);
            exit(1);
        }
    }

    // write .jpeg footer (end-of-image marker)
    huffman_stop();

    // close input/output files
    fclose(file_rgb);
    fclose(file_jpg);

    // return successfully
    return 0;
}
