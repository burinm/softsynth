/*  Voice.cpp - Invented Here License v1.0 (2017) - burin

    Implementation of Voice class 

*/

#include "Voice.h"
#include "utils/notes.h"

extern "C" {
#include <stdio.h> //TODO: remove, tesing only
#include "hardware.h"
#include "wave_function.h" //TODO: for random_reset, find better way to encapsulate this
}

#ifdef ARDUINO
    #include "debug.h"
#endif

namespace SoftSynth {

/*
    Formula: round(65536)/ticks) @ 62500.000 Hz
*/

const uint16_t Voice::note_phase_mult_table[128] = {
         9,     9,    10,    10,    11,    11,    12,    13,    14,    14,    15,    16,// C-1
        17,    18,    19,    20,    22,    23,    24,    26,    27,    29,    31,    32,// C0
        34,    36,    39,    41,    43,    46,    49,    52,    55,    58,    61,    65,// C1
        69,    73,    77,    82,    87,    92,    97,   103,   109,   116,   123,   130,// C2
       138,   146,   155,   164,   173,   184,   195,   206,   219,   232,   245,   260,// C3
       275,   292,   309,   328,   347,   368,   389,   413,   437,   463,   491,   520,// C4
       551,   584,   618,   655,   694,   735,   779,   825,   874,   926,   981,  1040,// C5
      1102,  1167,  1237,  1310,  1388,  1470,  1558,  1651,  1749,  1853,  1963,  2080,// C6
      2203,  2334,  2473,  2620,  2776,  2941,  3116,  3301,  3497,  3705,  3926,  4159,// C7
      4406,  4669,  4946,  5240,  5552,  5882,  6232,  6602,  6995,  7411,  7851,  8318,// C8
      8813,  9337,  9892, 10480, 11104, 11764, 12463, 13205
};

void Voice::init(uint8_t (*f)(uint16_t), envelope_t &e) {
    phase = 0;
    wave_function = f;
    current_note = 0;

    envelope.init(e);
}

void Voice::step(uint16_t t) {

    phase = t * note_phase_mult_table[GET_NOTE(current_note)];            //?us
    //phase = phase % PARTS_PER_CYCLE; //automatic 16bit rollover
    envelope.step();                                                //1.92us
}

void Voice::startNote(uint8_t midinote) {
    SET_NOTE(current_note,midinote);
    envelope.start();                                       //3.44us
    random_reset();
} 

void Voice::stopNote() {
    NOTE_OFF(current_note);
    envelope.setState(ADSR_RELEASE);
    //should we reset envelope here?
}

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

void Voice::setWaveform(uint8_t (*f)(uint16_t)) {
    wave_function = f;
}

}
