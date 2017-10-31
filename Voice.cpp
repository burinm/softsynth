/*  Voice.cpp - Invented Here License v1.0 (2017) - burin

    Implementation of Voice class 

*/

#include "Voice.h"

namespace SoftSynth {

void Voice::init(uint8_t (*f)(uint16_t), envelope_t &e) {

    wave_function = f;

    current_note = 0;
    sequencer_flag = 0;

    envelope.init(e);
}

void Voice::step() {

    if (sequencer_flag) {
        //if (//TODO:overflow sync
            sequencer_flag = 0;
            envelope.start();
        //}
    }

    envelope.step();


}

void Voice::startNote(uint8_t midinote) {
    current_note = midinote;
    sequencer_flag = 1;
} 

void Voice::stopNote() {
    sequencer_flag = 0;
    envelope.setState(ADSR_RELEASE);
}

uint8_t Voice::sample(uint16_t clock) {
uint8_t wave;
uint16_t amplitude;
uint16_t phase;

    phase = clock * note_phase_mult_table[current_note];
    phase = phase >> 6;

    wave = wave_function(phase);
    amplitude = envelope.apply_envelope(wave);     

return (uint8_t)amplitude;
}


void Voice::setWaveform(uint8_t (*f)(uint16_t)) {
    wave_function = f;
}

}
