#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "../source/common.h"


//  Enums
enum {SAW, SQUARE, SINE};

//  The tables
float *sinetable;
float *sawtable;


void create_sinetable(int wavetable_size){
    int n;
    for(n = 0; n < wavetable_size; n++){
        sinetable[n] = sin(TwoPi * (float)n/wavetable_size);
    }
}


void create_sawtable(int midi_note, int shape, int wavetable_size){
    //  Figure out how many partials this midi note has.
    float frequency = Tuning * pow(2, (midi_note - 69) / 12.0f);
    int   num_harmonics = (int)(Fs * 0.5f / frequency);
    int   harmonic_step = shape == SQUARE ? 2 : 1;
    int   n, h;
    float m;

    //  Zero this table
    for(n = 0; n < wavetable_size; n++){ sawtable[n] = 0.0f; }

    //  Each harmonic
    for(h = 0; h < num_harmonics; h += harmonic_step){
        int partial = h + 1;
        m = cos((float)h * HalfPi / (float)num_harmonics); // Gibbs
        m *= m;                                            // Reduction
        m /= (float)partial;                               // <- Right here

        for(n = 0; n < wavetable_size; n++){
            //  This is really interesting
            sawtable[n] += m * sinetable[(n*partial) % wavetable_size];
        }
    }
}


void normalize(float *buffer, int wavetable_size){
    int n;
    float max_value = 0.0f;
    for(n = 0; n < wavetable_size; n++){
        if(buffer[n] > max_value){ max_value = buffer[n]; }
    }
    for(n = 0; n < wavetable_size; n++){
        buffer[n] *= (1.0f / max_value);
    }
}


////
//  Main, Usage is ./program <midi_note> <sine|saw|square>
int main(int argc, char **argv){
    int n;
    int midi_note;
    int wavetable_size = 1024;

    float *output_table;

    if(argc < 4){
      printf("Usage: %s <midi_note> <sine|saw|square> <size>\n", argv[0]);
      exit(1);
    }

    //  Do up the arguments
    midi_note = atoi(argv[1]);

    wavetable_size = atoi(argv[3]);
    sinetable = (float*)malloc(sizeof(float) * wavetable_size);
    sawtable  = (float*)malloc(sizeof(float) * wavetable_size);

    //  Always need the sine table
    create_sinetable(wavetable_size);

    if(!strcmp("sine", argv[2])){
        output_table = sinetable;

    } else if(!strcmp("square", argv[2])){
        create_sawtable(midi_note, SQUARE, wavetable_size);
        output_table = sawtable;

    } else if(!strcmp("saw", argv[2])){
        create_sawtable(midi_note, SAW, wavetable_size);
        output_table = sawtable;

    }else{
      printf("Usage: %s <midi_note> <sine|saw|square> <size>\n", argv[0]);
      exit(1);
    }

    normalize(output_table, wavetable_size);

    //  Output
    for(n = 0; n < wavetable_size; n++){
        //  Now I am going to output these as 16-bit integers.
        int16_t sample = (int16_t)round(output_table[n] * 32767.0f);
        printf("%d\n", sample);
        if(n != wavetable_size - 1){ printf(", "); }
    }

    return 0;
}
