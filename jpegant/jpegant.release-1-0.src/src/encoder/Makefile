#CC=gcc
CFLAGS =-g -O -msse2 -fomit-frame-pointer
CPPFLAGS =-O3 -msse2
CFLAGS+=-Wa,-a=$<.s
LIBS =
OBJECTS = dct.o jpeg.o

jpegant: $(OBJECTS)
	$(CXX) $(CPPFLAGS) main.cpp $(OBJECTS) -o jpegant

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
                  
$(OBJECTS): arch.h
 
clean:
	rm -f $(OBJECTS) jpegant *.s
