/*  Envelope.h - (c) 2017 - burin

    Attack, Decay, Sustain, Release style
    envelope generator for 8-bit soft synth

*/

#ifndef __ENVELOPE_H__
#define __ENVELOPE_H__

extern "C" {
    #include <stdint.h> //TODO: avr-g++ doesn't support <cstdint>??
}

namespace SoftSynth {

typedef enum ADSR_STATE_TYPE { ADSR_OFF=0,
                       ADSR_ATTACK,
                       ADSR_DECAY,
                       ADSR_SUSTAIN,
                       ADSR_RELEASE
} adsr_state_t;


/*
  Envelope segment lengths go for x*y (ticks*counts),
   and will change the envelope_level by y counts.
    Slope = y/x
    When choosing values, the following constraints

         Envelope has 127 levels (y)
         127 => Total Attack => (Release + Decay)

    100ms = 4407 cycles (counts)
*/
typedef struct {
    uint16_t attack_ticks;
    uint8_t attack_count;

    uint16_t decay_ticks;
    uint8_t decay_count;

    uint16_t sustain_ticks;
    uint8_t sustain_hold;       // 0 = release, 1 = hold

    uint16_t release_ticks;
    uint8_t release_count;
} envelope_t;

class Envelope {

    public:
        static const uint8_t envelope_table[128];

        void init(envelope_t&);
        void start();
        void step();

        inline void setState(adsr_state_t s) { adsr_state = s; };
        inline adsr_state_t getState(void) { return adsr_state; };

        uint16_t apply_envelope(uint8_t);
        envelope_t  adsr_reset;    //Reset values for envelope

        void test_set_adsr_envelope_level(uint8_t l) {adsr_envelope_level = l;}

    private:
        uint8_t adsr_envelope_level;
        adsr_state_t adsr_state;

        envelope_t  adsr_run;      //Running count of envelope parameters
};

}

#endif
