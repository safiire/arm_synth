#include "midi_slicer.h"


MidiSlicer::MidiSlicer(void){
    m_last_status = 0;
}


////
//  Try to add a byte to the buffer
void MidiSlicer::add_byte(char byte){
    MidiCommandBuffer *write_buffer = m_ring_buffer.get_write_buffer();

    if(byte < 0x80){
        //  Process Data Bytes
        if(m_last_status == 0){ return; }
        //  If it is a data byte with no status yet, use last status
        if(write_buffer->bytes_stored == 0){
            write_buffer->command_size = message_size((m_last_status & 0xf0) >> 4);
            write_buffer->add_byte(m_last_status);
        }
        write_buffer->add_byte(byte);
        if(write_buffer->is_complete()){
            m_ring_buffer.advance_write_pointer();
        }
    }else if(byte >= 0x80 && byte <= 0xef){
        //  Voice Category Status bytes
        m_last_status = byte;
        write_buffer->command_size = message_size((byte & 0xf0) >> 4);
        write_buffer->add_byte(byte);
    }else if(byte >= 0xf0 && byte <= 0xf7){
        //  System Common Messages should clear stored last status byte
        //  I am ignoring them, as well as 0xf8 Real Time Messages for now
        m_last_status = 0;
    }
}


////
//  Get a copy of a full message in the buffer or returns false
bool MidiSlicer::get_midi_message(char *message_out){
    MidiCommandBuffer *read_buffer = m_ring_buffer.get_read_buffer();
    if(read_buffer->is_complete()){
        memcpy(message_out, read_buffer->command, read_buffer->bytes_stored);
        m_ring_buffer.advance_read_pointer();
        //m_ring_buffer.debug();
        return true;
    }
    return false;
}


////
//  Here is a table that lists each valid midi message size
inline char MidiSlicer::message_size(char message_number){
    static char message_sizes[] = {0,0,0,0,0,0,0,0,3,3,3,3,2,2,3,0};
    return message_sizes[(int)message_number];
}

