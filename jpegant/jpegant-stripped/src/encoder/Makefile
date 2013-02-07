#CC=gcc
CFLAGS =-O2 -std=c99 -D__X86
CFLAGS+=-Wa,-a=$<.s
LIBS =
OBJECTS = dct.o jpegenc.o rgbdata.o
HEADERS = dct.h jpegenc.h rgbdata.h

jpegant: $(OBJECTS) main.c
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o jpegant

$(OBJECTS): %.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@
          
# build program to corrupt .jpeg image for testing purposes
corruptor : % : %.c
	$(CC) -Wall -O2 $< -o $@ -lc

# check program output by compressing a raw RGB image and then
# writing the first 64 bytes of the resulting image the screen
# (note that each word stores results with bytes flipped, that
# is '0xABCD' is written as '0xCDAB')
test_image := jpeg_spacetest/640x480_test.rgb
check: jpegant $(test_image) corruptor
	./$< $(test_image) test.jpg
	@echo ':: huffman_start() ::'      >  test.out
	@od -x -j0    -N2    -w32 test.jpg >> test.out
	@echo ':: write_APP0info() ::'     >> test.out
	@od -x -j2    -N18   -w32 test.jpg >> test.out
	@echo ':: write_DQTinfo() ::'      >> test.out
	@od -x -j20   -N134  -w32 test.jpg >> test.out
	@echo ':: write_SOF0info(w,h) ::'  >> test.out
	@od -x -j154  -N19   -w32 test.jpg >> test.out
	@echo ':: write_DHTinfo() ::'      >> test.out
	@od -x -j173  -N420  -w32 test.jpg >> test.out
	@echo ':: write_DRIinfo() ::'      >> test.out
	@od -x -j593  -N6    -w32 test.jpg >> test.out
	@echo ':: write_SOSinfo() ::'      >> test.out
	@od -x -j598  -N14   -w32 test.jpg >> test.out
	@echo ':: start of compressed data ::' >> test.out
	@od -x -j612  -N8    -w32 test.jpg >> test.out
	./corruptor test.jpg corrupt.jpg

clean:
	rm -f $(OBJECTS) jpegant *.s
	rm -f test.jpg corrupt.jpg
