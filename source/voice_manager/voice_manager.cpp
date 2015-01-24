#include "voice_manager.h"


VoiceManager::VoiceManager(MidiManager *mm, int waveshape){
    int i;
    m_midi_manager = mm;
    //  Set all the midi notes and envelopes to not playing
    for(i = 0; i < 128; i++){
        m_notes_playing[i] = NULL;
        m_envelopes_running[i] = NULL;
        m_filter_envelopes_running[i] = NULL;
    }
    //  Let's signup to be called back for all MIDI for now.
    callback_request(MidiWildcard, MidiWildcard, MidiWildcard);
    //  Let's setup every oscillator we manage 
    for(i = 0; i < MAX_VOICES; i++){
        m_oscillators[i].set_waveshape(waveshape);
        m_oscillators[i].set_midi_manager(m_midi_manager); 
        m_envelopes[i].set_midi_manager(m_midi_manager);
        m_filter_envelopes[i].set_midi_manager(m_midi_manager);
    }
    m_filter_on = false;
}


////
//  Set the table on all this voice's oscillators
void VoiceManager::set_waveshape(int waveshape){
    int i;
    for(i = 0; i < MAX_VOICES; i++){
        m_oscillators[i].set_waveshape(waveshape);
    }
}


////
//  Here I accept external MIDI control
void VoiceManager::midi_callback(MidiMessage &msg){
    int i;
    int note = (int)msg.data1();

    //  Voices only care about NoteOn and NoteOff
    switch(msg.status()){
    case MidiNoteOn:
      //  Need to find an oscillator that isn't busy to play this note
      for(i = 0; i < MAX_VOICES; i++){
          if(!m_oscillators[i].is_playing()){
              m_oscillators[i].midi_callback(msg);
              m_notes_playing[note] = &m_oscillators[i];
              //  Volume Enevelopes
              m_envelopes[i].midi_callback(msg);
              m_envelopes[i].send_note_off_to(&m_oscillators[i]);
              m_envelopes_running[note] = &m_envelopes[i];
              //  Filter Envelopes
              m_filter_envelopes[i].midi_callback(msg);
              m_filter_envelopes_running[note] = &m_envelopes[i];
              break;
          }
          //  Otherwise I guess we're just not going to play it at all
      }
      break;
    case MidiNoteOff:
      // Find out who is playing this note and tell it
      if(m_notes_playing[note]){
          m_envelopes_running[note]->midi_callback(msg);
          m_envelopes_running[note] = NULL;
          //  Filter Envelopes
          m_filter_envelopes_running[note]->midi_callback(msg);
          m_filter_envelopes_running[note] = NULL;
          //  Note is now off
          m_notes_playing[note] = NULL;  
      }
      break;
    case MidiControlChange:
      if(msg.data1() == 1){  //  CC 1 is Modwheel, going to attach to filter cutoff
          m_lowpass.set_cutoff(msg.data2());
      }
      break;
    }
}


void VoiceManager::filter_on(bool b){
    m_filter_on = b;
}


////
//  Get the combined value of every oscillator
void VoiceManager::fill_buffer(int16_t *buffer, int size){
    int i, k;
    int16_t osc;
    uint16_t env;
    int32_t sample;

    for(k = 0; k < size; k++){
        buffer[k] = 0;
        for(i = 0; i < MAX_VOICES; i++){
            if(m_oscillators[i].is_playing()){
                //  The oscillator value / 8 so 8 voices at once won't clip
                osc = m_oscillators[i].value() >> 3;
                //  The envelope value, which is between 0 - 2^16.
                env = m_envelopes[i].value();
                //  sample should be osc * env / 2^16 which should also make it fit in an int16
                sample = osc * env;
                sample = sample >> 16;
                buffer[k] += (int16_t)sample;
            }
        }
        //m_lowpass.value(buffer[k]);
        buffer[k] = m_filter_on ? m_lowpass.value(buffer[k]) : buffer[k];
    }
}

