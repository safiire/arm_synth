#include "midi_ring_buffer.h"


////
//   Constructor zeros the buffer and sets pointers
MidiRingBuffer::MidiRingBuffer(void){
    memset(m_command_buffer, 0, sizeof(MidiCommandBuffer) * MIDI_BUFFER_SIZE);
    m_read_index = m_write_index = 0;
}


////
//   Return the read pointer 
MidiCommandBuffer *MidiRingBuffer::get_read_buffer(void){
    return &m_command_buffer[m_read_index];
}


////
//   Return the write pointer 
MidiCommandBuffer *MidiRingBuffer::get_write_buffer(void){
    return &m_command_buffer[m_write_index];
}


////
//  Advance the read pointer
void MidiRingBuffer::advance_read_pointer(void){
    m_read_index++;
    m_read_index &= 0xf;  // Wrap
}


////
//  Advance the write pointer
void MidiRingBuffer::advance_write_pointer(void){
    m_write_index++;
    m_write_index &= 0xf;
    memset(&m_command_buffer[m_write_index], 0, sizeof(MidiCommandBuffer));
}


////
//  Debug stuff
void MidiRingBuffer::debug(void){
    int i;
    SerialUSB.print("\x1B[2J"); // Clear screen ANSI

    SerialUSB.println("Midi Ring Buffer");
    SerialUSB.println("----------------");
    for(i = 0; i < MIDI_BUFFER_SIZE; i++){
        SerialUSB.print(i);
        SerialUSB.print(": [");
        SerialUSB.print((int)m_command_buffer[i].command[0]);
        SerialUSB.print(", ");
        SerialUSB.print((int)m_command_buffer[i].command[1]);
        SerialUSB.print(", ");
        SerialUSB.print((int)m_command_buffer[i].command[2]);
        SerialUSB.print(", ");
        SerialUSB.print((int)m_command_buffer[i].bytes_stored);
        SerialUSB.print(", ");
        SerialUSB.print((int)m_command_buffer[i].command_size);
        SerialUSB.print("]");

        if(m_write_index == i){
            SerialUSB.print(" <- WritePtr ");
        }

        if(m_read_index == i){
            SerialUSB.print(" <- ReadPtr ");
        }
        SerialUSB.println();
    }
}


