#include "rgbdata.h"

bool yuv_get_block(unsigned x, unsigned y, unsigned sx, unsigned sy, 
			YUV *buf, YUV *addr)
{
	if ((y + sy) > IMG_HEIGHT || (x + sx) > IMG_WIDTH) {
        	return false;
        }

        for (unsigned r = 0; r < sy; r++) {
                unsigned offset = (y+r)*IMG_WIDTH + x;
                for (unsigned c = 0; c < sx; c++) {
	                unsigned i = offset + c;
                        buf[sx*r + c] = addr[i];
                }
        }

        return true;
}

void close_yuv( char *filename, YUV *mmap_addr )
{
	struct stat sbuf;

	if(stat(filename,&sbuf) == -1){
                return;
        }

	munmap( mmap_addr, sbuf.st_size );
	return;
}

bool load_yuv( char *filename, YUV **mmap_addr )
{
	int fd;
	struct stat sbuf;

	if((fd = open(filename,O_RDONLY)) == -1){
		return false;
	}

	if(stat(filename,&sbuf) == -1){
		return false;
	}

	*mmap_addr = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, 
			fd, 0);
	
	if( *mmap_addr == (void*)-1){
		return false;
	}

	return true;
}