#include "stdint.h"
#include "rgbdata.h"
#include "dct.h"
#include "jpegenc.h"
#include "ch.h"
#include "hal.h"
//#include "spi_flash.h"


void write_jpeg(uint8_t* buff, unsigned size);

void jpeg_init(void);
void jpeg_close(void);

void rgb_get_block(uint16_t x, RGB *buf, uint8_t *input_buffer);

void convert_rows(uint8_t *input_buffer);
