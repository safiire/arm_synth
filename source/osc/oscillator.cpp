#include "oscillator.h"
#include "wavetables.h"

#define WrapMask4096 (0xfffffff)
#define WrapMask2048 (0x7ffffff)
#define WrapMask1024 (0x3ffffff)

////
//  A Matrix of wavetables :)
const int16* g_wavetable_data[NUM_OSCILLATORS][9] = {
    { NULL, sine_all_octaves, sine_all_octaves, sine_all_octaves, sine_all_octaves, sine_all_octaves, sine_all_octaves, sine_all_octaves, sine_all_octaves },
    { NULL, saw_octave1,      saw_octave2,      saw_octave3,      saw_octave4,      saw_octave5,      saw_octave6,      saw_octave7,      sine_all_octaves },
    { NULL, square_octave1,   square_octave2,   square_octave3,   square_octave4,   square_octave5,   square_octave6,   sine_all_octaves, sine_all_octaves }
};

////
//  This is the size of each wavetable by octave and waveshape
const uint32 g_wavetable_sizes[NUM_OSCILLATORS][9] = {
    {0,     4096,             4096,              4096,             4096,             4096,            4096,             4096,             4096},
    {0,     4096,             4096,              2048,             2048,             1024,            1024,             1024,             1024},
    {0,     4096,             4096,              2048,             2048,             1024,            1024,             1024,             1024} 
};

////
//   Rate/Tuning Matrix
const uint32 *g_rate_tuning[NUM_OSCILLATORS][9] = {
    {NULL,  tuning_4096,      tuning_4096,       tuning_4096,      tuning_4096,       tuning_4096,    tuning_4096,      tuning_4096,      tuning_4096},
    {NULL,  tuning_4096,      tuning_4096,       tuning_2048,      tuning_2048,       tuning_1024,    tuning_1024,      tuning_1024,      tuning_1024},
    {NULL,  tuning_4096,      tuning_4096,       tuning_2048,      tuning_2048,       tuning_1024,    tuning_1024,      tuning_1024,      tuning_1024},
};

////
//   Mask value to wrap at the edge of the wavetable
const uint32 g_wrap_mask[NUM_OSCILLATORS][9] = {
    {0,     WrapMask4096,      WrapMask4096,       WrapMask4096,    WrapMask4096,      WrapMask4096,   WrapMask4096,     WrapMask4096,     WrapMask4096},
    {0,     WrapMask4096,      WrapMask4096,       WrapMask2048,    WrapMask2048,      WrapMask1024,   WrapMask1024,     WrapMask1024,     WrapMask1024},
    {0,     WrapMask4096,      WrapMask4096,       WrapMask2048,    WrapMask2048,      WrapMask1024,   WrapMask1024,     WrapMask1024,     WrapMask1024},
};
  


//  Constructor
Oscillator::Oscillator(){
    m_is_playing = false;
    m_q16_one = 1 << 16;
    m_midi_note = 0;
    m_octave = 0;
    m_waveshape = SINE;
    m_wavetable_size = 4096;
    m_wavetable_wrap_mask = g_wrap_mask[m_waveshape][m_octave];
    m_rate = 0;
    m_position = 0;
}


//// 
//  Set the midi manager
void Oscillator::set_midi_manager(MidiManager *mm){
    m_midi_manager = mm;
}


////
//  Set the wavetable 
void Oscillator::set_waveshape(int waveshape){
    m_waveshape = waveshape;
}


////
//  Is this Oscillator available to play a new note?
bool Oscillator::is_playing(){
    return m_is_playing;
}


////
//  Here I accept external MIDI control
void Oscillator::midi_callback(MidiMessage &msg){

    switch(msg.status()){
    case MidiNoteOn:
        m_position = 0;
        m_midi_note = msg.data1();
        m_octave = (m_midi_note / 12) - 2; //  Integer Division
        m_wavetable_size = g_wavetable_sizes[m_waveshape][m_octave];
        m_rate = g_rate_tuning[m_waveshape][m_octave][m_midi_note];
        m_wavetable_wrap_mask = g_wrap_mask[m_waveshape][m_octave];
        m_current_wavetable = g_wavetable_data[m_waveshape][m_octave];
        m_is_playing = true;
        break;
    case MidiNoteOff:
        m_is_playing = false;
    }
}


//  Get the value at global_n
int16 Oscillator::value(void){

    //  This whole sequence + interpolation takes ~30 cycles
    //  I am allowed ~ 1632 cycles to generate one sample, this takes ~1.83% of that time.
    //  At worst I am wrong by a factor of like 2, and that is about 3.5%.
    //  Not worth optimizing.

    if(!m_is_playing){ return 0; }
    uint16 index_whole;
    uint16 index_next_whole;
    uint16 index_fract;
    uint32 next_position;

    //  Get next position in wavetable
    //  bitwise & wraps it between 0.0 - WavetableSize.99999...
    m_position += m_rate;
    next_position = m_position + m_q16_one;

    m_position &= m_wavetable_wrap_mask;
    next_position &= m_wavetable_wrap_mask;

    index_whole = m_position >> 16;
    index_next_whole = next_position >> 16;
    index_fract = m_position & 0xffff;
    return interpolate_linear(m_current_wavetable[index_whole], m_current_wavetable[index_next_whole], index_fract);
}


//  Integer linear interpolate between two integer values
inline int16 Oscillator::interpolate_linear(int16 pt1, int16 pt2, uint16 position){
    return ((pt1 * position) >> 16) + ((pt2 * (0xffff - position)) >> 16);
}


//  Class function to change Midi to Frequency
float Oscillator::midi_to_frequency(char midi_note){
    return Tuning * pow(2, (midi_note - 69) / 12.0f);
}
