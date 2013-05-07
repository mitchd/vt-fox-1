#
# makefile for testing jpeg encoder on the host machine
#

CC=gcc
CFLAGS = -Wall -O2 -std=c99 -DENABLE_RGB
OBJECTS = dct.host.o jpegenc.host.o
HEADERS = dct.h      jpegenc.h

jpegant: $(OBJECTS) test_encoder.c
	$(CC) $(CFLAGS) test_encoder.c $(OBJECTS) -o jpegant

generate_jpeg_header : % : %.c $(OBJECTS)
	$(CC) $(CFLAGS) $< $(OBJECTS) -o $@

$(OBJECTS): %.host.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

# image converter (24-bit/1-pixel RGB to 16-bit/1-pixel RGB)
convert_rgb24_to_rgb16 : % : %.c
	$(CC) $(CFLAGS) -o $@ $<

# image converter (24-bit/1-pixel RGB to 32-bit/2-pixel YUV)
convert_rgb24_to_yuv : % : %.c
	$(CC) $(CFLAGS) -o $@ $<

# convert image
640x480_test.rgb16 : %.rgb16 : %.rgb24 convert_rgb24_to_rgb16
	./convert_rgb24_to_rgb16 $< $@
          
# convert image
640x480_test.yuv : %.yuv : %.rgb24 convert_rgb24_to_yuv
	./convert_rgb24_to_yuv $< $@
          
# check program output by compressing raw RGB24, RGB16, and YUV
# images into .jpg files
check: jpegant 640x480_test.rgb24 640x480_test.rgb16 640x480_test.yuv
	./$< 640x480_test.rgb24 test_rgb24.jpg
	./$< 640x480_test.rgb16 test_rgb16.jpg
	./$< 640x480_test.yuv   test_yuv.jpg

# generate raw .jpeg header
jpeg_header.out : generate_jpeg_header
	./$<

clean:
	rm -f $(OBJECTS)
	rm -f jpegant convert_rgb24_to_rgb16 convert_rgb24_to_yuv
	rm -f 640x480_test.rgb16 640x480_test.yuv
	rm -f test_rgb24.jpg test_rgb16.jpg test_yuv.jpg

