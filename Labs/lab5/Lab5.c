#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ------------------------------------------------------
// HELPFUL DEBUGGING UTILITIES - NOT PART OF THE SOLUTION

void bprintInt( int n)
{
	int i;
	for (i = 31 ; i >=0  ; --i )
 	{
       if ( (n >> i) & 1)
          printf("1" );
       else
          printf("0" );
    }
}

void bprintByte( char n)
{
	int i;
	for (i = 7 ; i >=0  ; --i )
 	{
       if ( (n >> i) & 1)
          printf("1" );
       else
          printf("0" );
    }
}
void bprintShort( short n)
{
	int i;
	for (i = 15 ; i >=0  ; --i )
 	{
       if ( (n >> i) & 1)
          printf("1" );
       else
          printf("0" );
    }
}
// ------------------------------------------------------

int main( int argc, char *argv[] )
{
	FILE* txtFile, *binFile;
	int shortsReadIn=0,  _12bitValsWritten=0, _12bitValsReadBackIn;
	short s1,s2; /* the pair read each time */

	if (argc<3)
	{
		printf("usage: %s  <infilename> <outfilename> (i.e. ./a.out sine.txt sxine.bin)\n", argv[0] );
		return EXIT_FAILURE;
	}

	txtFile = fopen( argv[1], "r" );
	if (txtFile == NULL )
	{
		printf("Can't open %s for input. Program halting\n",argv[1] );
		return EXIT_FAILURE;
	}

	binFile = fopen( argv[2], "w" );
	if (binFile == NULL )
	{
		printf("Can't open %s for output. Program halting\n",argv[2] );
		return EXIT_FAILURE;
	}



	while((fscanf(txtFile, "%hi", &s1 ) == 1))  // giving you the read loop so you dont use incorrect idiom of  while !EOF
	{
		// * * * *  Y O U R   C O D E   H E R E * * * *
		// every iteration of this loop should read in two shorts from the text file and write three bytes to the binary file
		int readCount = fscanf(txtFile,"%hi",&s2); // #1
		if (readCount > 0) 
		{
			s1 = s1 << 4; // #2

			// #3
			short mask = 0x0F00;
			mask = (mask & s2) >> 8;
			s1 = s1 | mask;
			fwrite(&s1, 2, 1, binFile); // #4

			//#5
			mask = 0x00FF;
			s2 = s2 & mask;
			char c = s2;

			fwrite(&c, 1, 1, binFile); //#6
		} else { // if we are on the last num of odd number of values
			fwrite(&s1, 2, 1, binFile);
		}
	}

	printf("%d integers written to %s (3 bytes per 2 integers written)\nNow READING/ECHOING values from binary outfile\n",_12bitValsWritten, argv[2] );	fclose( txtFile );
	fclose( binFile );

/* READ BACK AND ECHO THE VALUES WRITTEN OUT */

	binFile = fopen( argv[2], "rb" );
	if (binFile == NULL )
	{
		printf("Can't open %s for input. Program halting\n",argv[2] );
		return EXIT_FAILURE;
	}

	while ( fread(&s1, sizeof(s1), 1, binFile) == 1 ) // giving you the correct form of the read so you don't use  bad-> while !EOF
	{
		// * * * *  Y O U R   C O D E   H E R E * * * *
		// every iteration of this loop should read in ONE shorts and ONE byte from the binary file
		// from that short and byte reconstitute and write to stdout the TWO oringinal shorts to
		// that were compressed into them.
		char c;
		int readCount = fscanf(binFile,"%c",&c);
		if (readCount > 0)
		{
			s2 = c;
			s2 = s2 & 0x00FF;

			short mask = 0x000F;
			mask = mask & s1;
			mask = mask << 12;
			mask = mask >> 4;
			s2 = mask | s2;

			s1 = s1 >> 4;
			mask = 0x0FFF;


			printf("%hi\n%hi\n", s1, s2);
		} else {
			printf("%hi\n", s1);
		}
	}
	fclose( binFile );

	return EXIT_SUCCESS;
}

