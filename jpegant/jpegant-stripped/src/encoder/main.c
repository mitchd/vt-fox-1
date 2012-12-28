#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#include "bitmap.h"
#include "dct.h"
#include "jpegenc.h"

/*void write_jpeg(const unsigned char buff[], const unsigned size)
{
	write(file_jpg, buff, size);
}*/


// chroma subsampling, i.e. converting a 16x16 RGB block into 8x8 Cb and Cr
void subsample(const BGR rgb[16][16], short cb[8][8], short cr[8][8])
{
	for (unsigned r = 0; r < 8; r++)
	for (unsigned c = 0; c < 8; c++)
	{
		unsigned rr = (r<<1);
		unsigned cc = (c<<1);

		// calculating average values
		color R = (rgb[rr][cc].Red + rgb[rr][cc+1].Red
				+ rgb[rr+1][cc].Red + rgb[rr+1][cc+1].Red) >> 2;
		color G = (rgb[rr][cc].Green + rgb[rr][cc+1].Green
				+ rgb[rr+1][cc].Green + rgb[rr+1][cc+1].Green) >> 2;
		color B = (rgb[rr][cc].Blue + rgb[rr][cc+1].Blue
				+ rgb[rr+1][cc].Blue + rgb[rr+1][cc+1].Blue) >> 2;

		cb[r][c] = (short)RGB2Cb(R, G, B)-128;
		cr[r][c] = (short)RGB2Cr(R, G, B)-128;
	}
}


int main (int argc, char *argv[])
{
	CBitmap bmp;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s file-in.bmp file-out.jpg\n", argv[0]);
		return -1;
	}

	if (!bmp.Load(argv[1])) {
		fprintf(stderr, "Error: cannot open %s\n", argv[1]);
		return -1;
	}

	if (bmp.GetBitCount() != 24) {
		fprintf(stderr, "Error BitCount != 24\n");
		return -1;
	}

	BGR   RGB16x16[16][16];
	short Y8x8[2][2][8][8]; // four 8x8 blocks - 16x16
	short Cb8x8[8][8];
	short Cr8x8[8][8];

	if ((file_jpg = open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, S_IWRITE|S_IREAD)) < 0) {
		fprintf(stderr, "Error: cannot create %s (%s)\n", argv[2], strerror(errno));
		return -1;
	}

	// Process image by 16x16 blocks, (16x16 because of chroma subsampling)
	// The resulting image will be truncated on the right/down side
	// if its width/height is not N*16.
	// The data is written into <file_jpg> file by write_jpeg() function
	// which Huffman encoder uses to flush its output, so this file
	// should be opened before the call of huffman_start().
	huffman_start(bmp.GetHeight() & -16, bmp.GetWidth() & -16);

	for (unsigned y = 0; y < bmp.GetHeight()-15; y += 16) {
		for (unsigned x = 0; x < bmp.GetWidth()-15; x += 16)
		{
			if (!bmp.GetBlock(x, y, 16, 16, (BGR*)RGB16x16)) {
				printf("Error: getBlock(%d,%d)\n", x, y);
				break;
			}

			// geting four 8x8 Y-blocks
			for (unsigned i = 0; i < 2; i++)
			for (unsigned j = 0; j < 2; j++)
			{
				for (unsigned r = 0; r < 8; r++)
				for (unsigned c = 0; c < 8; c++)
				{
					const unsigned rr = (i<<3) + r;
					const unsigned cc = (j<<3) + c;

					const color R = RGB16x16[rr][cc].Red;
					const color G = RGB16x16[rr][cc].Green;
					const color B = RGB16x16[rr][cc].Blue;
					// converting RGB into Y (luminance)
					Y8x8[i][j][r][c] = RGB2Y(R, G, B)-128;
				}
			}
			// getting subsampled Cb and Cr
			subsample(RGB16x16, Cb8x8, Cr8x8);

			// 1 Y-compression
			dct(Y8x8[0][0], Y8x8[0][0]);
			huffman_encode(HUFFMAN_CTX_Y, (short*)Y8x8[0][0]);
			// 2 Y-compression
			dct(Y8x8[0][1], Y8x8[0][1]);
			huffman_encode(HUFFMAN_CTX_Y, (short*)Y8x8[0][1]);
			// 3 Y-compression
			dct(Y8x8[1][0], Y8x8[1][0]);
			huffman_encode(HUFFMAN_CTX_Y, (short*)Y8x8[1][0]);
			// 4 Y-compression
			dct(Y8x8[1][1], Y8x8[1][1]);
			huffman_encode(HUFFMAN_CTX_Y, (short*)Y8x8[1][1]);
			// Cb-compression
			dct(Cb8x8, Cb8x8);
			huffman_encode(HUFFMAN_CTX_Cb, (short*)Cb8x8);
			// Cr-compression
			dct(Cr8x8, Cr8x8);
			huffman_encode(HUFFMAN_CTX_Cr, (short*)Cr8x8);
		}
	}

	huffman_stop();
	close(file_jpg);

	return 0;
}
