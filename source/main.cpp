#include "common.h"
#include "audio_codec/audio_codec_maple.h"
#include "wirish.h"
#include "midi/midi_slicer.h"
#include "midi/midi_manager.h"
#include "voice_manager/voice_manager.h"
#include "audio_buffer/audio_buffer.h"

bool DebugOn = false;
MidiSlicer midi_slicer;
MidiManager midi_manager;
VoiceManager voice1(&midi_manager, SQUARE);
AudioBuffer audio_buffer;

int16_t left_out    = 0; 
int16_t right_out   = 0;

void setup() {
    Serial3.begin(31250);
    if(!DebugOn){
        nvic_irq_disable(NVIC_USB_LP_CAN_RX0);
        SerialUSB.end();
        AudioCodec_init();
    }
}


void AudioCodec_interrupt(){
    int16_t v = audio_buffer.read();
    AudioCodec_data(NULL, NULL, v, v);
}


// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void premain() {
    init();
}


int main(void) {
    int16_t debug_buffer[BufferSize];
    char midi_message[3];
    uint16_t mod0_value = 0;
    uint16_t mod1_value = 0;
    setup();
    while (true) { 

        if(DebugOn){
            voice1.fill_buffer(debug_buffer, BufferSize);
        }else{
            if(audio_buffer.needs_samples()){
                voice1.fill_buffer(audio_buffer.get_back_buffer(), BufferSize);
            }
        }

        if(Serial3.available()){
            midi_slicer.add_byte(Serial3.read());
            if(midi_slicer.get_midi_message(midi_message)){
                midi_manager.process_midi(midi_message);
                if(DebugOn){
                    int i;
                    for(i = 0; i < BufferSize; i++){
                        SerialUSB.println(debug_buffer[i]);
                    }
                }
            }
        }

        AudioCodec_ADC(&mod0_value, &mod1_value);  

        if(mod1_value < 21845){
            voice1.set_waveshape(SINE);
        }else if(mod1_value > 43690){
            voice1.set_waveshape(SQUARE);
        }else{
            voice1.set_waveshape(SAW);
        }

        if(mod0_value < 32767){
            voice1.filter_on(false);
        }else{
            voice1.filter_on(true);
        }
    }
    return 0;
}

