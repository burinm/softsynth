/*  Voice.cpp - Invented Here License v1.0 (2017) - burin

    Implementation of Voice class 

*/

#include "Voice.h"

extern "C" {
#include "hardware.h"
}

#ifdef ARDUINO
    #include "debug.h"
#endif

namespace SoftSynth {

#include "voice_notes.h"

void Voice::init(uint8_t (*f)(uint16_t), envelope_t &e) {
    phase = 0;
    wave_function = f;
    #ifdef POLYPHONY
        pool_tiny_init(&note_pool);
    #else
        current_note = 0;
    #endif


    envelope.init(e);
}

void Voice::setWaveform(uint8_t (*f)(uint16_t)) {
    wave_function = f;
}

void Voice::setControl(uint8_t control, uint8_t value) {
    switch(control) {
        case   ATTACK_COUNT_CTRL:
            envelope.adsr_reset.attack_count = value;
            break;
        case   DECAY_COUNT_CTRL:
            envelope.adsr_reset.decay_count = value;
            break;
        case   SUSTAIN_COUNT_CTRL:
            envelope.adsr_reset.sustain_hold = value;
            break;
        case   RELEASE_COUNT_CTRL:
            envelope.adsr_reset.release_count = value;
            break;

        case   ATTACK_TICKS_CTRL:
            envelope.adsr_reset.attack_ticks = value;
            break;
        case   DECAY_TICKS_CTRL:
            envelope.adsr_reset.decay_ticks = value;
            break;
        case   SUSTAIN_TICKS_CTRL:
            envelope.adsr_reset.sustain_ticks = value;
            break;
        case   RELEASE_TICKS_CTRL:
            envelope.adsr_reset.release_ticks = value;
            break;
        default:
            break;
    }
}

}
