#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "../osc/oscillator.h"
#include "../midi/midi_manager.h"

class Envelope : public Object {

    private:
    enum {ATTACK, DECAY, SUSTAIN, RELEASE, CLOSED};
    int m_state;
    Oscillator *m_send_note_off_to;
    uint16 m_rate;
    uint16 m_target;
    int32  m_value;
    uint16 m_attack_rate;
    uint16 m_decay_rate;
    uint16 m_sustain_level;
    uint16 m_vel_sustain;
    uint16 m_release_rate;
    uint16 m_saved_velocity; 
    MidiMessage m_note_off;

    public:
    Envelope(void);
    void set_midi_manager(MidiManager *mm);
    void send_note_off_to(Oscillator *osc);
    virtual void midi_callback(MidiMessage &msg);
    uint16 value();
    void set_attack_rate(uint16 rate);
    void set_decay_rate(uint16 rate);
    void set_sustain_level(uint16 rate);
    void set_release_rate(uint16 rate);
    void set_attack_time(uint16 time);
    void set_decay_time(uint16 time);
    void set_release_time(uint16 time);
    void set_adsr(uint16 a, uint16 d, uint16 s, uint16 r);
    int  get_state(void);
};


#endif
