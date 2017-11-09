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

enum VOICE_CONTROLS { ATTACK_CTRL = 14,
                      DECAY_CTRL = 15,
                      SUSTAIN_CTRL = 16,
                      RELEASE_CTRL = 17
                    };

class Voice {
    public:
        static const uint16_t note_phase_mult_table[128];

        void init(uint8_t (*f)(uint16_t), envelope_t&);
        void startNote(uint8_t midinote);
        void stopNote();
        void step(uint16_t);

        void setWaveform(uint8_t (*wave_function)(uint16_t));
        void setControl(uint8_t control, uint8_t value);

#if 0
        inline uint8_t Voice::sample() {
            return (uint8_t)envelope.apply_envelope(wave_function(phase));
            //return (uint8_t)(wave_function(phase));
        }
#endif
        uint8_t sample();

    private:
        uint16_t phase;

        //Current note bit 7 is gate, bits 0-6 are note
        #define NOTE_GATE_BIT   7
        #define NOTE_GATE_MASK  (1 << NOTE_GATE_BIT)
        #define NOTE_OFF(u8) ((u8) &= ~NOTE_GATE_MASK)
        #define NOTE_ON(u8)  ((u8) |= NOTE_GATE_MASK)
        #define IS_NOTE_ON(u8)  ((u8) & NOTE_GATE_MASK)
        #define GET_NOTE(u8)    (u8) & ~NOTE_GATE_MASK
        //Setting note also turns on gate
        #define SET_NOTE(u8,note)    ((u8) = (note | (1<<NOTE_GATE_BIT)))
        uint8_t current_note;

        uint8_t (*wave_function)(uint16_t);
        Envelope envelope;
};


}
#endif
