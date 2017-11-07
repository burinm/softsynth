/*  Voice.h - Invented Here License v1.0 (2017) - burin

    Voice object for a soft synthesizer 

*/

#ifndef __VOICE_H__
#define __VOICE_H__

extern "C" {
    #include <stdint.h> //avr-g++ doesn't support <cstdint>??
}


#include "Envelope.h"

namespace SoftSynth {
class Voice {
    public:
        void init(uint8_t (*f)(uint16_t), envelope_t&);
        void startNote(uint8_t midinote);
        void stopNote();
        void step(uint16_t);

        void setWaveform(uint8_t (*wave_function)(uint16_t));

#if 0
        inline uint8_t Voice::sample() {
            return (uint8_t)envelope.apply_envelope(wave_function(phase));
            //return (uint8_t)(wave_function(phase));
        }
#endif
        uint8_t sample();

        uint8_t sequencer_flag;

    private:
        uint16_t ticks;
        uint16_t phase;
        uint16_t previous_phase;
        uint8_t current_note;
        uint8_t (*wave_function)(uint16_t);
        Envelope envelope;


/*
    Formula: round(65536)/ticks) @ 62500.000 Hz
*/

const uint16_t note_phase_mult_table[128] = {
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


};

}
#endif
