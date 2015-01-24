#include "midi_manager.h"


////
// Contstructor
MidiManager::MidiManager(){
    m_next_index = 0;
    memset(m_midi_message_requests, 0, sizeof(MidiMessageRequest_t) * MAX_MIDI_REQUESTS);
}


////
//  Process a midi message
void MidiManager::process_midi(char *midi_data){
    unsigned int i;
    MidiMessage midi(midi_data);

    for(i = 0; i < m_next_index; i++){
        //  TODO Just give midi to every requestor for now...
        m_midi_message_requests[i].m_requestor->midi_callback(midi);
   }
}


////
//  Request to be called back
void MidiManager::callback_request(MidiMessageRequest_t const &request){
  memcpy(&m_midi_message_requests[m_next_index], &request, sizeof(MidiMessageRequest_t));
  m_next_index++;
}



