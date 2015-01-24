#include "envelope.h"
#include "wirish.h"

////
//  Constructor
Envelope::Envelope(void) : m_note_off((char*)"\x80\x0\x40"){
  m_midi_manager = NULL;
  m_send_note_off_to = NULL;
  m_rate   = 0;
  m_target = 0;
  m_value  = 0;
  m_attack_rate = 66;  // was 66 This is 0.001f
  m_decay_rate  = 66; 
  m_sustain_level = 52428;  // This is 0.8f
  m_release_rate = 5; // This is 0.0001f;
  m_state = ATTACK;
}


////
//  Set the midi_manager
void Envelope::set_midi_manager(MidiManager *mm){
  m_midi_manager = mm;
}


////
//  Set the midi_manager
void Envelope::send_note_off_to(Oscillator *osc){
  m_send_note_off_to = osc;
}



////
//  Set the rate of attack 
void Envelope::set_attack_rate(uint16 rate){
    m_attack_rate = rate < 0 ? -m_rate : m_rate;
}


////
//  Set the rate of decay 
void Envelope::set_decay_rate(uint16 rate){
    m_decay_rate = rate < 0 ? -m_rate : m_rate;
}


////
//  Set the sustain level
void Envelope::set_sustain_level(uint16 level){
    m_sustain_level = level < 0 ? 0 : level;
}


////
//  Set the rate of release 
void Envelope::set_release_rate(uint16 rate){
    m_release_rate = rate < 0 ? -m_rate : m_rate;
}


////
//  Set the attack in time
void Envelope::set_attack_time(uint16 time){
    //  TODO this is wrong now
    // m_attack_rate = time < 0 ? (1 / (-time * 44100.0f)) : (1 / (time * 44100.0f));
}


////
//  Set the decay in time
void Envelope::set_decay_time(uint16 time){
    //  TODO this is wrong now
    //  m_decay_rate = time < 0.f ? (1.0 / (-time * 44100.0f)) : (1.0 / (time * 44100.0f));
}


////
//  Set the release in time
void Envelope::set_release_time(uint16 time){
    //  TODO this is wrong now
    //  m_release_rate = time < 0.f ? (m_sustain_level / (-time * 44100.0f)) : (m_sustain_level / (time * 44100.0f));
}


////
//  Set ADSR
void Envelope::set_adsr(uint16 a, uint16 d, uint16 s, uint16 r){
    //  TODO this is wrong now
    //  set_attack_time(a);
    //  set_decay_time(d);
    //  set_sustain_level(s);
    //  set_release_time(r);
}


////
//  Get the state
int Envelope::get_state(){
    return m_state;
}

////
//  Midi callback 
void Envelope::midi_callback(MidiMessage &msg){
    uint32 temp32;
    switch(msg.status()){
    case MidiNoteOn:
        m_saved_velocity = msg.data2();
        temp32   = 65535 * m_saved_velocity;
        temp32   = temp32 >> 7;
        m_target = (int16)temp32;  // Maybe it is best to do all of this in 32bit but only cast it at the end?
        temp32 = m_sustain_level * m_saved_velocity;
        temp32 = temp32 >> 7;
        m_vel_sustain = temp32;
        m_rate = m_attack_rate;
        m_state = ATTACK;
        break;
    case MidiNoteOff:
        m_target = 0;
        m_rate   = m_release_rate;
        m_state = RELEASE;
        break;
    }
}


////
//  Value of envelope
uint16 Envelope::value(){
    switch(m_state){
    case ATTACK:
      m_value += m_rate;
      if(m_value >= m_target){
        m_value  = m_target;
        m_rate   = m_decay_rate;
        m_target = m_sustain_level;
        m_state  = DECAY;
      }
      break;
    case DECAY:
      m_value -= m_decay_rate;
      if(m_value <= m_vel_sustain){
        m_value = m_vel_sustain;
        m_rate  = 0;
        m_state = SUSTAIN;
      }
      break;
    case RELEASE:
      m_value -= m_release_rate;
      if(m_value <= 0){
        m_value = 0;
        m_state = CLOSED;
      }
      break;
    case CLOSED:
      if(m_send_note_off_to){
          m_send_note_off_to->m_is_playing = false;  //  Quite a lot faster
          m_send_note_off_to = NULL;
          return 0;
      }
      break;
    }
    return (uint16)m_value;
}

