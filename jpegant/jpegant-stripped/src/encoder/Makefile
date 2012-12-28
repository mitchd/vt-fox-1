#CC=gcc
CFLAGS =-g -O2 
CPPFLAGS =-O2 
CFLAGS+=-Wa,-a=$<.s
LIBS =
OBJECTS = dct.o jpegenc.o

jpegant: $(OBJECTS)
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o jpegant

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
                  
clean:
	rm -f $(OBJECTS) jpegant *.s
