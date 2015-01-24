#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <stdint.h>
#include <math.h>
#include "lowpassf.h"

#define BLOCK_SIZE 512


int main(void){

  Lowpass filters[2];

  int i;
	SNDFILE *infile  = NULL;
	SNDFILE *outfile = NULL;
	SF_INFO	 sfinfo;
  sf_count_t readcount;
  int filetype = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  short buffer[BLOCK_SIZE * 2];

  //  Open a wav file to filter
	if((infile = sf_open ("music.wav", SFM_READ, &sfinfo)) == NULL) {	
      printf ("Not able to open input file\n");
      puts (sf_strerror (NULL)) ;
      return 1;
	}

  //  Say info about the input file
  printf("Frames: %d\n", (int)sfinfo.frames);
  printf("Samplerate: %d\n", sfinfo.samplerate);
  printf("Channels: %d\n", sfinfo.channels);

  sfinfo.format = filetype;
  //  Open output file
	if((outfile = sf_open ("output.wav", SFM_WRITE, &sfinfo)) == NULL) {	
      printf ("Not able to open input file\n");
      puts (sf_strerror (NULL)) ;
      return 1;
	}

  // Read and filter the file
	while((readcount = sf_readf_short(infile, buffer, BLOCK_SIZE)) > 0){
      for(i = 0; i < readcount * 2; i += 2){
          buffer[i]   =  filters[0].value(buffer[i]);
          buffer[i+1] =  filters[1].value(buffer[i+1]);
          //printf("%d ", buffer[i]);
          //printf("%d ", buffer[i+1]);
      }
      sf_writef_short(outfile, buffer, BLOCK_SIZE);
  }

  //  Close the files
  sf_close(infile);
  sf_close(outfile);

  return 0;
}
