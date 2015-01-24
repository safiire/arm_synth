#include "object.h"
#include "midi/midi_message_request.h"
#include "midi/midi_manager.h"

extern MidiManager midi_manager;

////
//  This baseclass should find the global midi manager and
//  assign it to every Object
Object::Object(){
    m_midi_manager = &midi_manager;
}


////
//   Every object has a callback for midi
void Object::midi_callback(MidiMessage &msg){
  //  Nothing by default
}


////
//   Every object can request a callback for a MIDI message
void Object::callback_request(int type, int channel, int cc){
  //MidiMessageRequest request(this, type, channel, cc);
  MidiMessageRequest_t request;
  request.m_message_types = type;
  request.m_channel = channel;
  request.m_control_change = cc;
  request.m_requestor = this;
  m_midi_manager->callback_request(request);
}
