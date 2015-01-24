#include "audio_buffer.h"
#include <string.h>


////
//  Contstructor
AudioBuffer::AudioBuffer(){
    memset(buffers, 0, sizeof(int16_t) * NumBuffers * BufferSize);
    active_buffer = buffers[0];
    back_buffer   = buffers[1];
    read_ptr      = active_buffer;
    buffer_end    = read_ptr + BufferSize; 
    b_needs_samples = true;
}


////
//  Read from the audio buffer
int16_t AudioBuffer::read(){
    int16_t v;
    v = *read_ptr;
    read_ptr++;
    if(read_ptr >= buffer_end){
        flip_buffers();
    }
    return v;
}


//  Does the backbuffer need new samples?
bool AudioBuffer::needs_samples(){
    return b_needs_samples;
}


////
//  Flip the buffers 
void AudioBuffer::flip_buffers(){
    int16_t *temp;
    temp = back_buffer;
    back_buffer = active_buffer;
    active_buffer = temp;
    read_ptr = active_buffer;
    buffer_end = read_ptr + BufferSize;
    b_needs_samples = true;
}



int16_t *AudioBuffer::get_back_buffer(){
    b_needs_samples = false;
    return back_buffer;
}

