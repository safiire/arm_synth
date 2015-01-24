#ifndef LOWPASS_H
#define LOWPASS_H

#include <stdint.h>
#include "../object.h"
#include "../common.h"


////  
//  Single pole low pass filter
class Lowpass : public Object {

    private:
    int32_t y_of_n_minus1;
    int32_t two_pi_over_fs;
    int32_t one;
    int32_t two;
    int32_t fs;
    int32_t pi;
    int32_t a0;
    int32_t b1;

    public:
    Lowpass();
    int16_t value(int16_t value);
    void set_cutoff(int fc);
    virtual void midi_callback(MidiMessage &msg);
};

#endif

//  H(z) =        a0 
//         ----------------
//          1 - b1 * z**-1
//
//  For a cutoff:
//
//  a0 = fc * 2PI / Fs
//  b0 = 1 - a0
