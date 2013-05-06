#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(){
  FILE * outfile = fopen( "test.rgb", "w" );
  uint16_t R = 0x7800;
  uint16_t G = 0x07E0;
  uint16_t B = 0x001F;
  uint16_t Bk = 0x0000;
  int row = 0;
  int column = 0;
  for( row; row < 240; row++ ){
    for( column = 0; column < 80; column++ )
      fwrite( &Bk, 2, 1, outfile );
    for( column = 0; column < 80; column++ )
      fwrite( &R, 2, 1, outfile );
    for( column = 0; column < 80; column++ )
      fwrite( &G, 2, 1, outfile );
    for( column = 0; column < 80; column++ )
      fwrite( &B, 2, 1, outfile );
  }

  fclose( outfile );
}
