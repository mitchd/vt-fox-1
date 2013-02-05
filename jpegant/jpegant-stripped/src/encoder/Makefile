#CC=gcc
CFLAGS =-O2 -std=c99 -D__X86
CFLAGS+=-Wa,-a=$<.s
LIBS =
OBJECTS = dct.o jpegenc.o rgbdata.o

jpegant: $(OBJECTS) main.c
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o jpegant

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
                  

# check program output by compressing 'image.rgb' and then
# writing the first 64 bytes of the resulting image the screen
# (note that each word stores results with bytes flipped, that
# is '0xABCD' is written as '0xCDAB')
check: jpegant image.rgb
	./$< image.rgb test.jpg
	@echo ':: huffman_start() ::'
	@od -x -j0    -N2    -w32 test.jpg
	@echo ':: write_APP0info() ::'
	@od -x -j2    -N18   -w32 test.jpg
	@echo ':: write_DQTinfo() ::'
	@od -x -j20   -N134  -w32 test.jpg
	@echo ':: write_SOF0info(w,h) ::'
	@od -x -j154  -N19   -w32 test.jpg
	@echo ':: write_DHTinfo() ::'
	@od -x -j173  -N420  -w32 test.jpg
	@echo ':: write_SOSinfo() ::'
	@od -x -j593  -N13   -w32 test.jpg
	@echo ':: start of compressed data ::'
	@od -x -j606  -N8    -w32 test.jpg

clean:
	rm -f $(OBJECTS) jpegant *.s
	rm -f test.jpg
