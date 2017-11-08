/*  Envelope.cpp - Invented Here License v1.0 (2017) - burin

    Implementation of Envelope class

*/

#include "Envelope.h"

namespace SoftSynth {

//Envelope power table
const uint8_t Envelope::envelope_table[128] = {
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

void Envelope::init(envelope_t &e) {

    adsr_envelope_level = 0;
    adsr_state = ADSR_OFF;

    //adsr_reset = e;
    adsr_reset.attack_ticks = e.attack_ticks;
    adsr_reset.attack_count = e.attack_count;

    adsr_reset.decay_ticks = e.decay_ticks;
    adsr_reset.decay_count = e.decay_count;

    adsr_reset.sustain_ticks = e.sustain_ticks;
    adsr_reset.sustain_hold = e.sustain_hold;

    adsr_reset.release_ticks = e.release_ticks;
    adsr_reset.release_count = e.release_count;

    

    adsr_run.attack_ticks = 0;
    adsr_run.attack_count = 0;

    adsr_run.decay_ticks = 0;
    adsr_run.decay_count = 0;

    adsr_run.sustain_ticks = 0;
    adsr_run.sustain_hold = 0;

    adsr_run.release_ticks = 0;
    adsr_run.release_count = 0;
}

void Envelope::start() {
    adsr_envelope_level = 0;
    adsr_state = ADSR_ATTACK;

    //adsr_run = adsr_reset; //5us!!
#if 1
    adsr_run.attack_ticks = adsr_reset.attack_ticks;
    adsr_run.attack_count = adsr_reset.attack_count;

    adsr_run.decay_ticks = adsr_reset.decay_ticks;
    adsr_run.decay_count = adsr_reset.decay_count;

    adsr_run.sustain_ticks = adsr_reset.sustain_ticks;
    adsr_run.sustain_hold = adsr_reset.sustain_hold;

    adsr_run.release_ticks = adsr_reset.release_ticks;
    adsr_run.release_count = adsr_reset.release_count;
#endif
}

void Envelope::step() {

    switch (adsr_state) {
        case ADSR_OFF:
        break;

        case ADSR_ATTACK:
            if(adsr_run.attack_ticks == 0) {
                if (adsr_run.attack_count == 0) { adsr_state=ADSR_DECAY; break;}
                adsr_envelope_level++; //volume up exponentially
                adsr_run.attack_count--;

                adsr_run.attack_ticks=adsr_reset.attack_ticks;
            }
            adsr_run.attack_ticks--;
        break;

        case ADSR_DECAY:
            if (adsr_run.decay_ticks == 0) {
                if (adsr_run.decay_count == 0) { adsr_state=ADSR_SUSTAIN; break;}
                adsr_envelope_level--; //volume down exponentially
                adsr_run.decay_count--;

                adsr_run.decay_ticks=adsr_reset.decay_ticks;
            }
            adsr_run.decay_ticks--;
        break;

        case ADSR_SUSTAIN:
            if (adsr_run.sustain_ticks == 0) {
                //Hold or continue state machine 
                if (adsr_run.sustain_hold == 0) { adsr_state=ADSR_RELEASE; break;}
            } else {
                adsr_run.sustain_ticks--;
            }
        break;

        case ADSR_RELEASE: //This state can be forced with stopNote()
            if (adsr_run.release_ticks == 0) {
                if (adsr_run.release_count == 0 || adsr_envelope_level == 0) {
                    adsr_state = ADSR_OFF;
                    adsr_envelope_level = 0; //Hmmm, should probably set sustain level first
                    //adsr_envelope_level = 0; //For now - eliminate later..
                    break;
                }
                adsr_envelope_level--; //volume down exponentially
                adsr_run.release_count--;

                adsr_run.release_ticks=adsr_reset.release_ticks;
            }
            adsr_run.release_ticks--;
        break;
    }

}

uint16_t Envelope::apply_envelope(uint8_t wave) {
uint16_t amplitude;

//TODO: Put all these constants in the hardware table?

    if (adsr_envelope_level == 0) {
        amplitude = 128;
    } else {
        //Positive voltage is 255 -> 127,  Adjusts 127 -> 0, result 128 <-> 255
        if (wave > 126) {
            amplitude = ((wave-127) *  envelope_table[adsr_envelope_level]) >>8;
            amplitude += 128;
        //Negative voltage is 126 -> 0, Adjusts 0 -> 126, result = 1 <-> 127
        } else {
            amplitude = ((127-wave) *  envelope_table[adsr_envelope_level]) >>8;
            amplitude = 127-amplitude;
        }
    }
return amplitude;
}


}

