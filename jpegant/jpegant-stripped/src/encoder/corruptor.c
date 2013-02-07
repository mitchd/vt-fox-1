//
// corruptor.c
//
// This application attempts to corrupt .jpeg files to test the restart
// intervals for data recovery.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

// 
int main (int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s file-in.jpg file-out.jpg\n", argv[0]);
		return -1;
	}

    // options
    unsigned int skip_len = 620;    // number of bytes to skip (.jpeg header)
    float error_rate = 0.002f;      // probability of corrupting a byte

    // try to load input file
    FILE * file_in = fopen(argv[1], "rb");
    if (!file_in) {
        fprintf(stderr,"error: %s, could not load input file '%s' for reading\n", argv[0], argv[1]);
        exit(1);
    }

    // try to load output file
    FILE * file_out = fopen(argv[2], "wb");
    if (!file_out) {
        fprintf(stderr,"error: %s, could not load output file '%s' for writing\n", argv[0], argv[2]);
        fclose(file_in);
        exit(1);
    }

    // ....
    int continue_running = 1;
    unsigned int  byte_counter = 0;
    unsigned char byte[1];
    unsigned int  num_corrupted = 0;
    while (continue_running) {
        // read input data (one byte at a time)
        int n = fread(byte, sizeof(unsigned char), 1, file_in);

        // ensure 
        if (n==0 && !feof(file_in)) {
            fprintf(stderr,"error: %s, could not read in data...\n", argv[0]);
            exit(1);
        }

        // update byte counter
        byte_counter += n;

        // determine if we should corrupt this byte
        if ( byte_counter >= skip_len &&
            ((float)rand() / (float)RAND_MAX) < error_rate )
        {
            byte[0] ^= 0x5c;    // flip some bits
            num_corrupted++;
        }
        n = fwrite(byte, sizeof(unsigned char), 1, file_out);

        // check exit criteria (end of file)
        if (feof(file_in))
            continue_running = 0;
    };

    // close files
    fclose(file_in);
    fclose(file_out);

    printf("skipped %u bytes (header), corrupted %u/%u total bytes\n",
            skip_len, num_corrupted, byte_counter);

    // finished
	return 0;
}
