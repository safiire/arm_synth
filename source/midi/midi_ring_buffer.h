#ifndef MIDI_RING_BUFFER
#define MIDI_RING_BUFFER
#include <string.h>
#include "wirish.h"

//  Ring buffer can hold up to 16 Midi commands
#define MIDI_BUFFER_SIZE 16

//  Let's make this more reasonable to read
class MidiCommandBuffer {
    public:
    char command[3];
    char bytes_stored;
    char command_size;
    bool is_empty(void){ return bytes_stored == 0; }
    bool is_complete(void){ return bytes_stored != 0 && bytes_stored == command_size; }
    void zero(void){ memset(this, 0, sizeof(MidiCommandBuffer)); };
    void add_byte(char byte){ command[(int)bytes_stored++] = byte; }
    char get_status_byte(void){ return (command[0] & 0xf0) >> 4; }
};


class MidiRingBuffer {

    private:
    MidiCommandBuffer  m_command_buffer[MIDI_BUFFER_SIZE];
    int m_read_index;
    int m_write_index;

    public:
    MidiRingBuffer(void);
    MidiCommandBuffer *get_read_buffer(void);
    MidiCommandBuffer *get_write_buffer(void);
    void advance_read_pointer(void);
    void advance_write_pointer(void);
    void debug(void);
};

#endif 



