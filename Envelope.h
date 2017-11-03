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


//Envelope segment lengths go for x*y (ticks*counts),
// and will change the envelope_level by y counts.
//  Slope = y/x
//  When choosing values, the following constraints
//
//         Envelope has 127 levels (y)
//         127 => Total Attack => (Release + Decay)
//
//  100ms = 4407 cycles (counts)
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
        void init(envelope_t&);
        void start();
        void step();

        inline void setState(adsr_state_t s) { adsr_state = s; };
        inline adsr_state_t getState(void) { return adsr_state; };

        uint16_t apply_envelope(uint8_t);

    private:
        uint8_t adsr_envelope_level;
        adsr_state_t adsr_state;

        envelope_t  adsr_run;      //Running count of envelope parameters
        envelope_t  adsr_reset;    //Reset values for envelope



};


//Envelope power table
static const uint8_t envelope_table[128] = {
    0,    2,    2,    3,    3,    3,    3,    3,
    3,    3,    3,    4,    4,    4,    4,    4,
    4,    5,    5,    5,    5,    5,    6,    6,
    6,    6,    6,    7,    7,    7,    8,    8,
    8,    8,    9,    9,    9,   10,   10,   11,
   11,   11,   12,   12,   13,   13,   14,   14,
   15,   15,   16,   17,   17,   18,   18,   19,
   20,   21,   21,   22,   23,   24,   25,   26,
   27,   28,   29,   30,   31,   32,   33,   34,
   35,   37,   38,   39,   41,   42,   44,   46,
   47,   49,   51,   53,   55,   57,   59,   61,
   63,   65,   68,   70,   73,   75,   78,   81,
   84,   87,   90,   94,   97,  100,  104,  108,
  112,  116,  120,  125,  129,  134,  139,  144,
  149,  154,  160,  166,  172,  178,  185,  191,
  198,  205,  213,  221,  229,  237,  246,  255
};


}




#endif
