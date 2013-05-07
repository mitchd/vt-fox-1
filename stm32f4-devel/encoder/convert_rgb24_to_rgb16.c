//
// convert_rgb24_to_rgb16.c
//
// This application converts uncompressed image 24-bit raw RGB image
// file to a compressed 16-bit raw RGB image
//

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// 
int main (int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s file-in.rgb24 file-out.rgb16\n", argv[0]);
		return -1;
	}

    // try to load input file
    FILE * file_in = fopen(argv[1], "rb");
    if (!file_in) {
        fprintf(stderr,"error: %s, could not load input file '%s' for reading\n", argv[0], argv[1]);
        exit(1);
    }

    // try to load output file
    FILE * file_out = fopen(argv[2], "wb");
    if (!file_out) {
        fprintf(stderr,"error: %s, could not load output file '%s' for writing\n", argv[0], argv[2]);
        fclose(file_in);
        exit(1);
    }

    // ....
    uint8_t rgb24[3];
    uint8_t rgb16[2];
    while (1) {
        // read input data (three bytes at a time)
        int num_read = fread(rgb24, sizeof(uint8_t), 3, file_in);

        // check if end of file, otherwise ensure data were read properly
        if (feof(file_in)) {
            break;
        } else if (num_read != 3) {
            fprintf(stderr,"error: %s, could not read in data from '%s'\n", argv[0], argv[1]);
            exit(1);
        }

        // convert 24-bit RGB pixel to compact 16-bit RGB pixel with
        // 5 bits for red, 6 bits for green, and 5 bits for blue:
        // (retain most-significant bits)
        //  [r7 r6 r5 r4 r3 g7 g6 g5|g4 g3 g2 b7 b6 b5 b4 b3]
        uint8_t red   = rgb24[0];
        uint8_t green = rgb24[1];
        uint8_t blue  = rgb24[2];

        rgb16[0] = (red & 0xf8) | ((green >> 5) & 0x07);
        rgb16[1] = ((green << 3) & 0xe0) | ((blue >> 3) & 0x1f);

        int num_written = fwrite(rgb16, sizeof(uint8_t), 2, file_out);
        if (num_written != 2) {
            fprintf(stderr,"error: %s, could not write data...\n", argv[0]);
            exit(1);
        }
    };

    // close files
    fclose(file_in);
    fclose(file_out);

    // finished
	return 0;
}
