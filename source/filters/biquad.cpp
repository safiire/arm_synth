#include "biquad.h"


////
//  Constructor
Biquad::Biquad(){
    x_of_n_minus1 = 0;
    x_of_n_minus2 = 0;
    y_of_n_minus1 = 0;
    y_of_n_minus2 = 0;
    set_cutoff(Fs / 4.0);
}


////
//  Change the cutoff frequency
void Biquad::set_cutoff(float fc){
    if(fc < 20){ fc = 20; }
    f = fc;
    r  = 1.4142135623730951; //sqrt(2) is lowest resonance - 0.1 is highest
    c  = 1.0 / tan(PI * f / (float)Fs);
    a0 = 1.0 / ( 1.0 + r * c + c * c);
    a1 = 2* a0;
    a2 = a0;
    b1 = 2.0 * ( 1.0 - c*c) * a0;
    b2 = ( 1.0 - r * c + c * c) * a0;
}


////
//  Get the value of the filtered sample - 2nd Order Biquad
int16 Biquad::value(int16 input){
    float x, y;

    x = (float)(input / 32767.0);
    y = (a0 * x) + (a1 * x_of_n_minus1) + (a2 * x_of_n_minus2) - (b1 * y_of_n_minus1) - (b2 * y_of_n_minus2);

    x_of_n_minus2 = x_of_n_minus1;
    x_of_n_minus1 = x;

    y_of_n_minus2 = y_of_n_minus1;
    y_of_n_minus1 = y;

    //  Make this int16 again
    return (int16)(y * 32767);
}



////
//  Get the value of the filtered sample
void Biquad::midi_callback(MidiMessage &msg){
    //  Nothin yet...
    switch(msg.status()){
    case MidiControlChange:

        break;
    };
}

