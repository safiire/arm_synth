#ifndef MIDI_SLICER_H
#define MIDI_SLICER_H

#include <wirish.h>
#include <string.h>
#include "midi_ring_buffer.h"

class MidiSlicer {

    private:
    MidiRingBuffer  m_ring_buffer;
    char m_last_status;
    char message_size(char message_number);
    
    public:
    MidiSlicer(void);
    void add_byte(char byte);
    bool get_midi_message(char *message_in);
};

#endif
