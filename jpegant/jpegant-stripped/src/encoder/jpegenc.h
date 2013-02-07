#ifndef __JPEG_H__
#define __JPEG_H__

#include <stdint.h>
#include <stdbool.h>

extern int file_jpg;

//---------------- J P E G ---------------

// Application should provide this function for JPEG stream flushing
extern void write_jpeg(const unsigned char buff[], const unsigned size);

typedef struct huffman_s
{
	const unsigned char  (*haclen)[12];
	const unsigned short (*hacbit)[12];
	const unsigned char  *hdclen;
	const unsigned short *hdcbit;
	const unsigned char  *qtable;
	short                dc;
}
huffman_t;

extern huffman_t huffman_ctx[3];

#define	HUFFMAN_CTX_Y	&huffman_ctx[0]
#define	HUFFMAN_CTX_Cb	&huffman_ctx[1]
#define	HUFFMAN_CTX_Cr	&huffman_ctx[2]

void huffman_start(short height, short width);
void huffman_stop(void);
void huffman_encode(huffman_t *const ctx, const short data[64]);

// write re-start interval termination character
void write_RSI();

#endif//__JPEG_H__
