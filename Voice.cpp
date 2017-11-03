/*  Voice.cpp - Invented Here License v1.0 (2017) - burin

    Implementation of Voice class 

*/

#include "Voice.h"
#include "utils/notes.h"

#ifdef ARDUINO
    #include "debug.h"
#endif

namespace SoftSynth {

void Voice::init(uint8_t (*f)(uint16_t), envelope_t &e) {
    ticks = 0;
    phase = 0;

    wave_function = f;

    current_note = 0;
    sequencer_flag = 0;

    envelope.init(e);
}

void Voice::step() {

    phase = ticks * note_phase_mult_table[current_note];            //3.62us
    phase = phase >> PHASE_MULT;
    phase &= 0x3ff;

    //if (ticks == note_ticks_table[current_note] ) { ticks = 0; }

    if (sequencer_flag) {
        //if (//TODO:overflow sync
            sequencer_flag = 0;
            envelope.start();                                       //3.44us
        //}
    }

    envelope.step();                                                //1.92us
    ticks++;

}

void Voice::startNote(uint8_t midinote) {
    current_note = midinote;
    sequencer_flag = 1;
} 

void Voice::stopNote() {
    sequencer_flag = 0;
    envelope.setState(ADSR_RELEASE);
}

#if 1
uint8_t Voice::sample() {
uint8_t wave;
uint16_t amplitude;

    if (envelope.getState() ) {
        wave = wave_function(phase);                                    //2.4us
        //amplitude = envelope.apply_envelope(wave);                      //2.4us
    } else {
        amplitude = 0;
    }

//return (uint8_t)amplitude;
return (wave);
}
#endif


void Voice::setWaveform(uint8_t (*f)(uint16_t)) {
    wave_function = f;
}

}
