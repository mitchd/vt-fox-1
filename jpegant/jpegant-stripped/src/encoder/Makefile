#CC=gcc
CFLAGS =-O2 -std=c99 -D__X86
CFLAGS+=-Wa,-a=$<.s
LIBS =
OBJECTS = dct.o jpegenc.o rgbdata.o

jpegant: $(OBJECTS)
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o jpegant

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
                  
clean:
	rm -f $(OBJECTS) jpegant *.s
