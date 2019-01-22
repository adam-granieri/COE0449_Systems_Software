/*
  project-1.c

  MY NAME: Adam Granieri
  MY PITT EMAIL: adg88@pitt.edu

  As you develop and test this file:

  use this command to compile: (you can name the executable whatever you like)
  gcc -W -Wall -Wextra -O2   project-1.c   -o project-1.exe

  use this command to execute:  (you will of course test on both input files)

  ./project-1.exe  sine-1.bin 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[])
{
  FILE * inFile= NULL;

  // DECLARE ALL ADDITIONAL NEEDED VARIABLES

  /* recommend declaring two short int i.e prev & current so that you can compare
     them to look for zero crossings or changes in direction and sign.
     You will also need some int counters etc.
  */
  short int prev, curr, totalCount = 1, waveCount = 0, waveSampleCount = 0, max = 0, min = 0, start = 0;

  if (argc < 2)
    {
      fprintf(stderr,"\nMust specify a binary file on the command line. Please try again.\n");
      exit(EXIT_FAILURE);
    }
  if  ((inFile = fopen(argv[1],"rb")) == NULL)
    {
      fprintf(stderr,"Can't open %s for input\n", argv[1] );
      exit(EXIT_FAILURE);
    }

  // YOUR CODE HERE - READ EACH VALUE FROM THE BINARY FILE ONE AT A TIME AND LOOK FOR ZERO CROSSINGS TO DETECT WAVES

  /* recommended strategy:
    \read in the first value of the file (prev) before the loop starts.
    \Then in the while loop read next short in.
    \In the loop you are to be looking for critical events:  zero crossings or sign changes.
    \Before you start writing that code start out just echoing each value and the sample # associated with it.
    \Once that is right start printing an alert whenever zero is touched/crossed OR or the direction changes.
    \Zero crossings and direction changes are the critical events you must correctly detect.
    \Once that is right then try to detect the start of the first wave.
    \Once that is right add code to  detect the end of the first wave.
     Then start coding to detet every wave - counting samples per wave and
     keeping track of the highest and lowest value in that wave.
  */

  fread( &prev, sizeof(prev), 1, inFile);

  while ( fread( &curr, sizeof(curr), 1, inFile) == 1 )
  {
  	totalCount++;

  	//Check to see if we're in a wave
  	if ( waveCount > 0 )
  	{
  		if ( curr > max )
  		{
  			max = curr;
  		}
  		else if (curr < min)
  		{
  			min = curr;
  		}
  	}

  	//Detected start of wave
  	if ( curr >= 0 && prev < 0 )
  	{
  		waveSampleCount = totalCount - start + 1;
  		//Check if it is also the end of the previous wave
  		if (curr == 0 && waveCount > 0)
  		{
  			printf("%hd\t%hd\t%hd\t%hd\t%hd\n", totalCount, curr, waveSampleCount, max, min);
  		}
  		else if ( curr != 0 && waveCount > 0)
  		{
  			printf("%hd \t%hd\t%hd \t%hd\t%hd\n", totalCount-1, prev, waveSampleCount-1, max, min);
  		}

  		start = totalCount;

  		waveCount++;
  		max = curr;
  		min = curr;
  	}

  	prev = curr;
  } 

  fclose(inFile); /* after the read loop is done. close file */

  return EXIT_SUCCESS;  // this is just a zero value. In C a zero return often indicates no error
}
