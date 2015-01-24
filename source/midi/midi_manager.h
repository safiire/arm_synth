#ifndef MIDI_MANAGER_H
#define MIDI_MANAGER_H

#include <wirish.h>
#include "midi_message_request.h"
#include "midi_message.h"
#include "string.h"

#define MAX_MIDI_REQUESTS 3

class MidiManager{

    public:
    MidiManager();
    void callback_request(MidiMessageRequest_t const &request);
    void process_midi(char *midi_data);

    private:
    unsigned int m_next_index;
    MidiMessageRequest_t m_midi_message_requests[MAX_MIDI_REQUESTS];
};

#endif 
