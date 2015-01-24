#ifndef MIDI_MESSAGE_H
#define MIDI_MESSAGE_H

enum MidiMessageType {MidiNoteOff = 0x8, MidiNoteOn, MidiPolyAftertouch, MidiControlChange, MidiProgramChange, MidiChannelAftertouch, MidiPitchWheel};

//  Base class for a MIDI message
class MidiMessage {
    protected:
    int  m_status;
    char m_channel;
    char m_data1;
    char m_data2;

    public:
    MidiMessage(char *midi_data);
    char channel();
    char data1();
    char data2();
    int  status();
};


//  NoteOff 
class NoteOff : public MidiMessage{
    public:
    char note_number();
    char velocity();
};


//  NoteOn 
class NoteOn : public MidiMessage{
    char note_number();
    char velocity();
};


//  PolyAftertouch 
class PolyAftertouch : public MidiMessage{
    char note_number();
    char pressure();
};


//  ControlChange 
class ControlChange : public MidiMessage{
    char control_number();
    char data();
};


//  ProgramChange 
class ProgramChange : public MidiMessage{
    char program_number();
};


//  ChannelAftertouch 
class ChannelAftertouch : public MidiMessage{
    char pressure();
};


//  PitchWheel 
class PitchWheel : public MidiMessage{
    int value();
};


#endif
