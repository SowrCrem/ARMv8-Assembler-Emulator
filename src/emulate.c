#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define NELEMENTS ((int) pow(2,18))

int main( int argc, char **argv ) {
    // Declaring Array to store binary instructions
    long data[NELEMENTS];

    if( argc != 2 ) {
        fprintf( stderr, "Usage: ./emulate filename!\n" );
        exit(1);
    }

    FILE *fp = fopen( argv[1], "rb" );

    // Error check for opening file
    if( fp == NULL ) {
        fprintf( stderr, "cat: can’t open %s\n", argv[1] );
        exit(1);
    }

    // Reading binary file
    fread( data, sizeof(long), NELEMENTS, fp);

    if( ferror(fp) ) {
        fprintf( stderr, "Error occurred reading from output.txt\n" );
        exit(1);
    }

    // Outputting contents of array storing binary file instructions
    for (int i=0; i < NELEMENTS; i++) {
        printf("%ld\n", data[i]);
    }

    return 0;
}

