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
        void step();

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
        uint8_t current_note;
        uint8_t (*wave_function)(uint16_t);
        Envelope envelope;
                

/*
    Formula: round((4096 << 5)/ticks) @ 11025.000 Hz
*/

const uint16_t note_phase_mult_table[128] = {
        97,   103,   109,   116,   123,   130,   138,   146,   154,   164,   173,   184,
       194,   206,   218,   231,   245,   260,   275,   291,   309,   327,   347,   367,
       389,   412,   437,   463,   490,   519,   550,   583,   617,   654,   693,   734,
       778,   824,   873,   925,   980,  1038,  1100,  1166,  1235,  1308,  1386,  1469,
      1556,  1649,  1747,  1850,  1960,  2077,  2201,  2331,  2470,  2617,  2772,  2937,
      3112,  3297,  3493,  3701,  3921,  4154,  4401,  4663,  4940,  5234,  5545,  5875,
      6224,  6594,  6986,  7402,  7842,  8308,  8802,  9325,  9880, 10467, 11090, 11749,
     12448, 13188, 13972, 14803, 15683, 16616, 17604, 18651, 19760, 20935, 22180, 23499,
     24896, 26376, 27945, 29606, 31367,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0
};



};

}
#endif
