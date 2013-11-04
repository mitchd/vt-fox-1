//
// convert_rgb24_to_yuv.c
//
// This application converts uncompressed image 24-bit/1-pixel RGB
// image file to a 32-bit/2-pixel YUV image
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
    uint8_t rgb24[6];   // 2 pixels @ 24-bit RGB
    uint8_t yuv[4];     // 2 pixels @ 16-bit YUV
    while (1) {
        // read input data (three bytes at a time)
        int num_read = fread(rgb24, sizeof(uint8_t), 6, file_in);

        // check if end of file, otherwise ensure data were read properly
        if (feof(file_in)) {
            break;
        } else if (num_read != 6) {
            fprintf(stderr,"error: %s, could not read in data...\n", argv[0]);
            exit(1);
        }

        // run conversion
        short red0   = rgb24[0];
        short green0 = rgb24[1];
        short blue0  = rgb24[2];

        short red1   = rgb24[3];
        short green1 = rgb24[4];
        short blue1  = rgb24[5];

        // convert RGB to YCbCr
        short y0  = (32768 + 19595*red0 + 38470*green0 + 7471*blue0) >> 16;
        short y1  = (32768 + 19595*red1 + 38470*green1 + 7471*blue1) >> 16;

        // average red/green/blue over adjacent pixels
        short red   = (red0   + red1  ) / 2;
        short green = (green0 + green1) / 2;
        short blue  = (blue0  + blue1 ) / 2;
        short cb = (8421376 - 11058*red - 21709*green + 32767*blue) >> 16;
        short cr = (8421376 + 32767*red - 27438*green -  5329*blue) >> 16;

        // save results
        yuv[0] = y0;
        yuv[1] = cb;
        yuv[2] = y1;
        yuv[3] = cr;

        int num_written = fwrite(yuv, sizeof(uint8_t), 4, file_out);
        if (num_written != 4) {
            fprintf(stderr,"error: %s, could not write data to '%s' (%d written)\n", argv[0], argv[2], num_written);
            exit(1);
        }
    };

    // close files
    fclose(file_in);
    fclose(file_out);

    // finished
	return 0;
}
