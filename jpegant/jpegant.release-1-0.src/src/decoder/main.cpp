#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "jpegdec.h"
#include "idct.h"
#include "bitmap.h"

#define CACHE_ALIGN __declspec(align(32))

#define CHECK(n) (((n) < 0)? 0: (((n) > 255)? 255: (n)))

inline color YCbCr2R(const short y, const short cb, const short cr)
{
	short ret = y + 1.402*(cr-128);
	return CHECK(ret);
}

inline color YCbCr2G(const short y, const short cb, const short cr)
{
	short ret = y - 0.34414*(cb-128) - 0.71414*(cr-128);
	return CHECK(ret);
}

inline color YCbCr2B(const short y, const short cb, const short cr)
{
	short ret = y + 1.772*(cb-128);
	return CHECK(ret);
}

void upsample(BGR rgb[16][16], const short Y[2][2][8][8],
		const short Cb[8][8], const short Cr[8][8])
{
	unsigned i, j;

	for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++)
	{
		unsigned r, c;

		for (r = 0; r < 8; r += 2)
		for (c = 0; c < 8; c += 2)
		{
			const unsigned rr = (i<<3) + r;
			const unsigned cc = (j<<3) + c;
			// convert pixels back into RGB
			const short cb = Cb[rr>>1][cc>>1] + 128;
			const short cr = Cr[rr>>1][cc>>1] + 128;
			short y;

			y = Y[i][j][r][c] + 128;
			rgb[rr][cc].Red   = YCbCr2R(y, cb, cr);
			rgb[rr][cc].Green = YCbCr2G(y, cb, cr);
			rgb[rr][cc].Blue  = YCbCr2B(y, cb, cr);

			y = Y[i][j][r][c+1] + 128;
			rgb[rr][cc+1].Red   = YCbCr2R(y, cb, cr);
			rgb[rr][cc+1].Green = YCbCr2G(y, cb, cr);
			rgb[rr][cc+1].Blue  = YCbCr2B(y, cb, cr);

			y = Y[i][j][r+1][c] + 128;
			rgb[rr+1][cc].Red   = YCbCr2R(y, cb, cr);
			rgb[rr+1][cc].Green = YCbCr2G(y, cb, cr);
			rgb[rr+1][cc].Blue  = YCbCr2B(y, cb, cr);

			y = Y[i][j][r+1][c+1] + 128;
			rgb[rr+1][cc+1].Red   = YCbCr2R(y, cb, cr);
			rgb[rr+1][cc+1].Green = YCbCr2G(y, cb, cr);
			rgb[rr+1][cc+1].Blue  = YCbCr2B(y, cb, cr);
		}
	}
}


int main (int argc, char *argv[])
{
	CBitmap           bmp;
	jpeg_dec_t        *jpeg;
	BGR               RGB16x16[16][16];
	CACHE_ALIGN short Y8x8[2][2][8][8]; // four 8x8 blocks - 16x16
	CACHE_ALIGN short Cb8x8[8][8];
	CACHE_ALIGN short Cr8x8[8][8];

	if (argc < 3) {
		fprintf(stderr, "Usage: %s file-in.jpg file-out.bmp\n", argv[0]);
		return -1;
	}

	jpeg = jpegdec_create(argv[1]);

	if (jpegdec_read_headers(jpeg))
	{
		jpeg_info_t info;

		jpegdec_get_info(jpeg, &info);

		bmp.Create(info.width, info.height);

		for (unsigned y = 0; y < info.height-15; y += 16)
		for (unsigned x = 0; x < info.width-15; x += 16)
		{
			jpegdec_decode(jpeg, 0, (short*)Y8x8[0][0]);
			idct(Y8x8[0][0], Y8x8[0][0]);

			jpegdec_decode(jpeg, 0, (short*)Y8x8[0][1]);
			idct(Y8x8[0][1], Y8x8[0][1]);

			jpegdec_decode(jpeg, 0, (short*)Y8x8[1][0]);
			idct(Y8x8[1][0], Y8x8[1][0]);

			jpegdec_decode(jpeg, 0, (short*)Y8x8[1][1]);
			idct(Y8x8[1][1], Y8x8[1][1]);

			jpegdec_decode(jpeg, 1, (short*)Cb8x8);
			idct(Cb8x8, Cb8x8);

			jpegdec_decode(jpeg, 2, (short*)Cr8x8);
			idct(Cr8x8, Cr8x8);

			upsample(RGB16x16, Y8x8, Cb8x8, Cr8x8);

			// save pixels
			if (!bmp.SetBlock(x, y, 16, 16, (BGR*)RGB16x16)) {
				printf("Error: SetBlock(%d,%d)\n", x, y);
			}
		}

		bmp.Save(argv[2]);
	}
	else {
		fprintf(stderr, "Error: cannot parse jpeg headers\n");
	}

	return 0;
}
