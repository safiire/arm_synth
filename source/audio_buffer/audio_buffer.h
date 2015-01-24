#ifndef AUDIO_BUFFER
#define AUDIO_BUFFER

#include <stdint.h>
#include "../common.h"

#define NumBuffers 2

class AudioBuffer {

    private:
    void flip_buffers();
    int16_t buffers[NumBuffers][BufferSize];
    int16_t *active_buffer;
    int16_t *buffer_end;
    int16_t *read_ptr;
    int16_t *back_buffer;
    bool b_needs_samples;

    public:
    AudioBuffer();
    int16_t *get_back_buffer();
    int16_t read();
    bool needs_samples();
};

#endif
