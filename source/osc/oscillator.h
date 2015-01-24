#ifndef OSCILLATOR
#define OSCILLATOR

#include "../common.h"
#include "../object.h"
#include "../midi/midi_manager.h"


enum {SINE, SAW, SQUARE, NUM_OSCILLATORS};


//  Every Oscillator is an Object
class Oscillator : public Object {

    private:
    uint32 m_wavetable_wrap_mask;
    uint32 m_q16_one;
    int m_midi_note;
    int m_octave;
    int m_waveshape;
    int m_wavetable_size;
    const int16  **m_wavetable_data;
    const int16   *m_current_wavetable;
    uint32 m_rate;
    uint32 m_position;


    public:
    Oscillator(void);
    int16 value(void);
    void set_midi_manager(MidiManager *mm);
    void set_waveshape(int waveshape);
    const int16 *current_wavetable();
    int16 interpolate_linear(int16 pt1, int16 pt2, uint16 position);
    static float midi_to_frequency(char midi_note);
    virtual void midi_callback(MidiMessage &msg);
    bool is_playing(void);
    bool m_is_playing;  //  Easier to stop an oscillator just by writing to this.
};

#endif
