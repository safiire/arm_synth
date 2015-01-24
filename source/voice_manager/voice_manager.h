#ifndef VOICEMANAGER
#define VOICEMANAGER

#include "../osc/oscillator.h"
#include "../envelope/envelope.h"
#include "../filters/biquad.h"
#include "../filters/lowpass.h"
#include <stdint.h>


class VoiceManager : public Object {


    public:
    VoiceManager(MidiManager *mm, int waveshape);
    void set_waveshape(int waveshape);
    void fill_buffer(int16_t *buffer, int size);
    void set_midi_manager(MidiManager *mm);
    void midi_callback(MidiMessage &msg);
    void filter_on(bool b);

    //  Let's just have one filter for now
    Biquad  m_biquad;
    Lowpass m_lowpass;

    private:
    //  Should have some number of oscillators to manage
    Oscillator  m_oscillators[MAX_VOICES];
    Oscillator *m_notes_playing[128];
    Envelope    m_envelopes[MAX_VOICES];
    Envelope   *m_envelopes_running[128];
    Envelope    m_filter_envelopes[MAX_VOICES];
    Envelope   *m_filter_envelopes_running[128];
    bool m_filter_on;
};

#endif
