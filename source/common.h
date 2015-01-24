#ifndef __COMMON_H

#define Fs 44100
#define WavetableSize 4096 
#define WavetableSizeF ((float)WavetableSize)
#define Pi 3.141592653589793
#define TwoPi 6.283185307179586f
#define HalfPi 1.5707963267948966f
#define TwoPiOverFs (TwoPi / ((float)Fs))
#define Tuning 440
#define SAMPLE_RATE 44
#define ADCS 2
#define BufferSize 512
#define MAX_VOICES 8   //  Remember that more than 8 can clip.

#endif
