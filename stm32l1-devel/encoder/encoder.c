#include "stdint.h"
#include "dct.h"
#include "jpegenc.h"
#include "spi_flash.h"
#include "uart_iface.h"

// constants specific to 640 x 480 image
#define IMG_WIDTH   (640)   // image width [pixels]
#define IMG_HEIGHT  (480)   // image height [pixels]
#define NUM_LINES   (60)    // number of 8-pixel lines in image

uint32_t file_addr_ptr = IMAGE_DATA_START;

void write_jpeg(uint8_t* buff, unsigned size)
{
    //sdWrite( IHU_UART_DEV, buff, size );
    flashWriteBytes( file_addr_ptr, buff, size );
    file_addr_ptr += (uint32_t)size;
}

uint32_t jpeg_addr_ptr(void)
{
    return file_addr_ptr;
}

// initialize jpeg encoder
void jpeg_init(void)
{
    // write the .jpg header
    // NOTE: this is no longer necessary because the image never
    //       changes size and the Huffman tables are fixed so there
    //       is no need to actually send this information with each
    //       image.
#ifndef RELEASE
    huffman_start(IMG_HEIGHT & -8, IMG_WIDTH & -8);
#endif
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
