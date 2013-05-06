#include "encoder.h"

//#include "spi_flash.h"

uint32_t file_addr_ptr = 0;

void write_jpeg(uint8_t* buff, unsigned size){
  sdWrite( &SD3, buff, size );
  //flashWriteBytes( file_addr_ptr, buff, size );
  file_addr_ptr += size;
}
color getRed( RGB values ){
  color red = (values.A & 0xF8);
  return red;
}

color getBlue( RGB values ){
  color blue = (values.B & 0x1F)<<3;
  return blue;
}

color getGreen( RGB values ){
  color green = (values.A & 0x07) << 5;
  green |= (values.B & 0xE0) >> 5;
  return green;
}

inline color RGB2Y(const color r, const color g, const color b){
  return (32768 + 19595*r + 38470*g + 7471*b) >> 16;
}
inline color RGB2Cb(const color r, const color g, const color b){
  return (8421376 - 11058*r - 21709*g + 32767*b) >> 16;
}
inline color RGB2Cr(const color r, const color g, const color b){
  return (8421376 + 32767*r - 27438*g - 5329*b) >> 16;
}


// chroma conversion
void convert(RGB rgb[8][8], short y[8][8], short cb[8][8], short cr[8][8]){
  RGB pixel;
  uint8_t r,c;
  for (r = 0; r < 8; r++)
  for (c = 0; c < 8; c++){
    pixel = rgb[r][c];
    cb[r][c] = (short)RGB2Cb( getRed(pixel), getGreen(pixel), getBlue(pixel) )-128;
    cr[r][c] = (short)RGB2Cr( getRed(pixel), getGreen(pixel), getBlue(pixel) )-128;
    y[r][c] = (short)RGB2Y( getRed(pixel), getGreen(pixel), getBlue(pixel))-128;
  }
}

void jpeg_init(void){
  huffman_start(IMG_HEIGHT & -8, IMG_WIDTH & -8);
}

void jpeg_close(void){
  huffman_stop();
}

void rgb_get_block(uint16_t x, RGB *buf, uint8_t *input_buffer){
  uint8_t r;
  uint16_t c, i, offset;
  for (r = 0; r < 8; r++) {
    offset = r*IMG_WIDTH + x;
    for (c = 0; c < 8; c++) {
      i = offset + c;
      buf[8*r + c].A = input_buffer[2*i];
      buf[8*r + c].B = input_buffer[2*i+1];
    }
  }
  return;
}

void convert_rows(uint8_t *input_buffer){
  short Y8x8[8][8]; // four 8x8 blocks - 16x16
  short Cb8x8[8][8];
  short Cr8x8[8][8];
  RGB   RGB8x8[8][8];

  // Process image by 8x8 blocks
  // The resulting image will be truncated on the right/down side
  // if its width/height is not N*8.
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
