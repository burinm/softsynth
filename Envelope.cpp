/*  Envelope.cpp - Invented Here License v1.0 (2017) - burin

    Implementation of Envelope class

*/

#include "Envelope.h"

namespace SoftSynth {

void Envelope::init(envelope_t &e) {

    adsr_envelope_level = 0;
    adsr_state = ADSR_OFF;

    adsr_reset = e;

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

    adsr_run = adsr_reset;
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

        case ADSR_RELEASE:
            if (adsr_run.release_ticks == 0) {
                if (adsr_run.release_count == 0) {
                    adsr_state = ADSR_OFF;
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

void Envelope::setState(adsr_state_t s) {
    adsr_state = s;
}


uint16_t Envelope::apply_envelope(uint8_t wave) {
uint16_t amplitude;

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

