#!/usr/bin/env ruby
require './scripts/audio.rb'

## 
#  Grab the #define'd values out of my header file
macros = get_c_macros

##
#  Let's make one fine 4096 sample sine table
`build/mkwavetable 69 sine 4096 > source/osc/sine_table.inc`
#  => 8192 bytes


##
#  And 8 variable width saw tables for each octave
#  These numbers are the midi value of each B note in the 8 octaves
#  Because a B has the least Harmonics below Nyquist of any other
#  ((4096 *2 ) + (2048 * 2) + (1024 * 3)) * 2
#  => 30720 bytes
`build/mkwavetable 47  saw 4096 > source/osc/saw_octave1.inc` # 178 Harmonics
`build/mkwavetable 59  saw 4096 > source/osc/saw_octave2.inc` #  89 Harmonics
`build/mkwavetable 71  saw 2048 > source/osc/saw_octave3.inc` #  44 Harmonics
`build/mkwavetable 83  saw 2048 > source/osc/saw_octave4.inc` #  22 Harmonics
`build/mkwavetable 95  saw 1024 > source/osc/saw_octave5.inc` #  11 Harmonics
`build/mkwavetable 107 saw 1024 > source/osc/saw_octave6.inc` #   5 Harmonics
`build/mkwavetable 119 saw 1024 > source/osc/saw_octave7.inc` #   2 Harmonics
`build/mkwavetable 127 saw 1024 > source/osc/saw_octave8.inc` #   1 Harmonic, So, a sine, already got that one.


##
#  And 8 variable width square tables for each octave
#  These numbers are the midi value of each B note in the 8 octaves
#  Because a B has the least Harmonics below Nyquist of any other note in an octave
#  Square waves only have odd harmonics.
#  ((4096 *2 ) + (2048 * 2) + (1024 * 2)) * 2
#  => 30720 bytes
`build/mkwavetable 47  square 4096 > source/osc/square_octave1.inc` #  89 Harmonics
`build/mkwavetable 59  square 4096 > source/osc/square_octave2.inc` #  45 Harmonics
`build/mkwavetable 71  square 2048 > source/osc/square_octave3.inc` #  22 Harmonics
`build/mkwavetable 83  square 2048 > source/osc/square_octave4.inc` #  11 Harmonics
`build/mkwavetable 95  square 1024 > source/osc/square_octave5.inc` #   6 Harmonics
`build/mkwavetable 107 square 1024 > source/osc/square_octave6.inc` #   3 Harmonics
`build/mkwavetable 119 square 1024 > source/osc/square_octave7.inc`#    1 Harmonic, A sine.
`build/mkwavetable 127 square 1024 > source/osc/square_octave8.inc` #   1 Harmonic, So, a sine, already got that one.


#  Total wavetable size is 59,392 bytes


##
#  Time to create the tuning/rate files for each wavetable size
`scripts/mktuning.rb 4096 > source/osc/tuning_4096.inc`
`scripts/mktuning.rb 2048 > source/osc/tuning_2048.inc`
`scripts/mktuning.rb 1024 > source/osc/tuning_1024.inc`


##
#  Let's make a C++ file that contains all the lookup tables for compilation
cpp_file =<<CPP

#include <stdint.h>
#include "wirish.h"

//  Tuning/Wavetable rates for each note
const uint32  tuning_4096[128] __FLASH__ = {
    #include "tuning_4096.inc"
};

const uint32  tuning_2048[128] __FLASH__ = {
    #include "tuning_2048.inc"
};

const uint32  tuning_1024[128] __FLASH__ = {
    #include "tuning_1024.inc"
};

//  Octave Lookup


//  Sine
const int16  sine_all_octaves[4096] __FLASH__ = {
    #include "sine_table.inc"
};

//  Saw
const int16  saw_octave1[4096] __FLASH__ = {
    #include "saw_octave1.inc"
};

const int16  saw_octave2[4096] __FLASH__ = {
    #include "saw_octave2.inc"
};

const int16  saw_octave3[2048] __FLASH__ = {
    #include "saw_octave3.inc"
};

const int16  saw_octave4[2048] __FLASH__ = {
    #include "saw_octave4.inc"
};

const int16  saw_octave5[1024] __FLASH__ = {
    #include "saw_octave5.inc"
};

const int16  saw_octave6[1024] __FLASH__ = {
    #include "saw_octave6.inc"
};

const int16  saw_octave7[1024] __FLASH__ = {
    #include "saw_octave7.inc"
};

const int16  saw_octave8[1024] __FLASH__ = {
    #include "saw_octave8.inc"
};


//  Square
const int16  square_octave1[4096] __FLASH__ = {
    #include "square_octave1.inc"
};

const int16  square_octave2[4096] __FLASH__ = {
    #include "square_octave2.inc"
};

const int16  square_octave3[2048] __FLASH__ = {
    #include "square_octave3.inc"
};

const int16  square_octave4[2048] __FLASH__ = {
    #include "square_octave4.inc"
};

const int16  square_octave5[1024] __FLASH__ = {
    #include "square_octave5.inc"
};

const int16  square_octave6[1024] __FLASH__ = {
    #include "square_octave6.inc"
};

const int16  square_octave7[1024] __FLASH__ = {
    #include "square_octave7.inc"
};

const int16  square_octave8[1024] __FLASH__ = {
    #include "square_octave8.inc"
};

CPP

File.open('source/osc/wavetables.h', 'w') do |fp|
    fp.write(cpp_file);
end


