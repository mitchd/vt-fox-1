#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <errno.h>
#include "rgbdata.h"
#include "dct.h"
#include "jpegenc.h"

int file_jpg;

void write_jpeg(const unsigned char buff[], const unsigned size)
{
	write(file_jpg, buff, size);
}

// chroma subsampling, i.e. converting a 16x16 RGB block into 8x8 Cb and Cr
void subsample(YUV yuv[8][16], short cb[8][8], short cr[8][8])
{
	for (unsigned r = 0; r < 8; r++)
	for (unsigned c = 0; c < 16; c+=2)
	{
		cb[r][c/2] = yuv[r][c].UV-128;
		cr[r][c/2] = yuv[r][c+1].UV-128;
	}
}


int main (int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s file-in.bmp file-out.jpg\n", argv[0]);
		return -1;
	}

	YUV *mmap_addr = NULL;

	if (!load_yuv(argv[1],&mmap_addr)) {
		fprintf(stderr, "Error: cannot open %s\n", argv[1]);
		return -1;
	}

	short Y8x8[2][8][8]; // four 8x8 blocks - 16x16
	short Cb8x8[8][8];
	short Cr8x8[8][8];
        YUV   YUV8x16[8][16];

	if ((file_jpg = open(argv[2], O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR )) < 0) {
		fprintf(stderr, "Error: cannot create %s (%i)\n", argv[2], errno);
		return -1;
	}

	// Process image by 8x16 blocks
	// The resulting image will be truncated on the right/down side
	// if its width/height is not N*8.
	// The data is written into <file_jpg> file by write_jpeg() function
	// which Huffman encoder uses to flush its output, so this file
	// should be opened before the call of huffman_start().
	huffman_start(IMG_HEIGHT & -8, IMG_WIDTH & -8);
	for (unsigned y = 0; y < IMG_HEIGHT; y += 8) {
		for (unsigned x = 0; x < IMG_WIDTH; x += 16)
		{
			if (!yuv_get_block(x, y, 16, 8, (YUV*)YUV8x16,
				mmap_addr)) {
				printf("Error: getBlock(%d,%d)\n", x, y);
				break;
			}

			for (unsigned r = 0; r < 8; r++)
			for (unsigned c = 0; c < 8; c++)
			{
				Y8x8[0][r][c] = YUV8x16[r][c].Y-128;
				Y8x8[1][r][c] = YUV8x16[r][c+8].Y-128;
			}
		
			//getting Cb and Cr
			subsample(YUV8x16, Cb8x8, Cr8x8);

			// 1 Y-compression
			dct(Y8x8[0], Y8x8[0]);
			huffman_encode(HUFFMAN_CTX_Y, (short*)Y8x8[0]);
			// 2 Y-compression
			dct(Y8x8[1], Y8x8[1]);
                        huffman_encode(HUFFMAN_CTX_Y, (short*)Y8x8[1]);
			// 1 Cb-compression
			dct(Cb8x8, Cb8x8);
			huffman_encode(HUFFMAN_CTX_Cb, (short*)Cb8x8);
			// 1 Cr-compression
			dct(Cr8x8, Cr8x8);
			huffman_encode(HUFFMAN_CTX_Cr, (short*)Cr8x8);
		}
	}

	huffman_stop();
	close(file_jpg);
	close_yuv(argv[1],mmap_addr);
	return 0;
}
