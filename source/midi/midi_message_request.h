#ifndef MIDI_MESSAGE_REQUEST_H
#define MIDI_MESSAGE_REQUEST_H

#include "../object.h"
#include "midi_message.h"

#define MidiWildcard -1

//  Here is a version which won't use dynamic memory in 
//  MidiManager
typedef struct {
    Object *m_requestor;
    int m_message_types;
    int m_channel;
    int m_control_change;
} MidiMessageRequest_t;

#endif
