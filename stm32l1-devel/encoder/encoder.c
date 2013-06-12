#include "stdint.h"
#include "dct.h"
#include "jpegenc.h"
#include "spi_flash.h"
#include "uart_iface.h"

// constants specific to 640 x 480 image
#define IMG_WIDTH   (640)   // image width [pixels]
#define IMG_HEIGHT  (480)   // image height [pixels]
#define NUM_LINES   (60)    // number of 8-pixel lines in image

uint32_t file_addr_ptr = 0;

#ifdef RELEASE
void write_jpeg(uint8_t* buff, unsigned size)
{
    sdWrite( IHU_UART_DEV, buff, size );
}
#else
void write_jpeg(const unsigned char buff[], const unsigned size)
{
    //flashWriteBytes( file_addr_ptr, buff, size );
    //file_addr_ptr += size;
    sdWrite( IHU_UART_DEV, (uint8_t*)&buff, size );
}
#endif //RELEASE

// initialize jpeg encoder
void jpeg_init(void)
{
    // write the .jpg header
    // NOTE: this is no longer necessary because the image never
    //       changes size and the Huffman tables are fixed so there
    //       is no need to actually send this information with each
    //       image.
    //huffman_start(IMG_HEIGHT & -8, IMG_WIDTH & -8);

    // reset the DC values
    huffman_resetdc();
}

// flush buffer
void jpeg_close(void)
{
    // stop Huffman encoding
    // NOTE: this flushes the buffer and writes the 'end of image'
    //       marker; even though the 'huffman_start()' method is
    //       not called, it is still important to call huffman_stop()
    huffman_stop();
}

// TODO:
// write yuv_get_line() method and use the following
// method to encode the line:
//
//  encode_line_yuv(line_buffer, line_number);
//
// where 'line_buffer' is an array of 10240 bytes and
// 'line_number' designates which of the 60 image
// lines is being encoded
//
// See 'encoder/test_encoder.c' for details



// 
// old methods no longer used
//
#if 0
void rgb_get_block(uint16_t x, RGB *buf, uint8_t *input_buffer){
  uint8_t r;
  uint16_t c, i, offset;
  for (r = 0; r < 8; r++) {
    offset = r*IMG_WIDTH + x;
    for (c = 0; c < 8; c++) {
      i = offset + c;
      buf[8*r + c].Red = input_buffer[i];
      buf[8*r + c].Green = input_buffer[i+1];
      buf[8*r + c].Blue = input_buffer[i+2];
    }
  }
  return;
}

void convert_rows(uint8_t input_buffer[8][IMG_WIDTH*3]){
  short Y8x8[8][8]; // four 8x8 blocks - 16x16
  short Cb8x8[8][8];
  short Cr8x8[8][8];
  RGB   RGB8x8[8][8];

  // Process image by 8x8 blocks
  // The resulting image will be truncated on the right/down side
  // if its width/height is not N*8.
  // The data is written into <file_jpg> file by write_jpeg() function
  // which Huffman encoder uses to flush its output, so this file
  // should be opened before the call of huffman_start().
  uint16_t x;
  for (x = 0; x < IMG_WIDTH; x += 8){
    rgb_get_block(x, (RGB*)RGB8x8, (uint8_t*)input_buffer);
    //Gonvert RGB to Y Cb Cr
    convert(RGB8x8, Y8x8, Cb8x8, Cr8x8);
    // 1 Y-compression
    dct(Y8x8, Y8x8);
    huffman_encode(HUFFMAN_CTX_Y, (short*)Y8x8);
    // 1 Cb-compression
    dct(Cb8x8, Cb8x8);
    huffman_encode(HUFFMAN_CTX_Cb, (short*)Cb8x8);
    // 1 Cr-compression
    dct(Cr8x8, Cr8x8);
    huffman_encode(HUFFMAN_CTX_Cr, (short*)Cr8x8);
  }
  
  return;
}
#endif
