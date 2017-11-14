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

    adsr_run.attack_ticks = adsr_reset.attack_ticks;
    adsr_run.attack_count = adsr_reset.attack_count;

    adsr_run.decay_ticks = adsr_reset.decay_ticks;
    adsr_run.decay_count = adsr_reset.decay_count;

    adsr_run.sustain_ticks = adsr_reset.sustain_ticks;
    adsr_run.sustain_hold = adsr_reset.sustain_hold;

    adsr_run.release_ticks = adsr_reset.release_ticks;
    adsr_run.release_count = adsr_reset.release_count;
}


#if 0
void Envelope::step() {
    adsr_envelope_level = 127;
}
#endif

#if 1
void Envelope::step() {

    switch (adsr_state) {
        case ADSR_OFF:
        break;

        case ADSR_ATTACK:
            if(adsr_run.attack_ticks == 0) {
                if (adsr_run.attack_count == 0) { adsr_state=ADSR_DECAY; break;}
                adsr_envelope_level++; //volume up
                adsr_run.attack_count--;

                adsr_run.attack_ticks=adsr_reset.attack_ticks;
            }
            adsr_run.attack_ticks--;
        break;

        case ADSR_DECAY:
            if (adsr_run.decay_ticks == 0) {
                if (adsr_run.decay_count == 0) { adsr_state=ADSR_SUSTAIN; break;}
                adsr_envelope_level--; //volume down
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
                    break;
                }
                adsr_envelope_level--; //volume down
                adsr_run.release_count--;

                adsr_run.release_ticks=adsr_reset.release_ticks;
            }
            adsr_run.release_ticks--;
        break;
    }

}
#endif

#if 0
//I think this scaling was weird all along
    was taking half the amplitude +/= and scaling by 0-255

uint16_t Envelope::apply_envelope(uint8_t wave) {
uint16_t amplitude;

/* This routine scales the amplitude according to the levels in
    envelope_table[level]. Amplitudes represented internally
    are values 0-255. The DC blocking cap (47uF) will offset
    the signal so that the middle values are around 0 volts.
    
    Choose:
           255->128 positive voltage
                127 to be the middle,
           126-> 0  negative voltage

    This routine scales the AC output from both sides around 
     the middle - an envelope, as opposed to just multiplying
     the signal by the scale
*/


//TODO: Put all these constants in the hardware table?

    if (adsr_envelope_level == 0) {
        amplitude = 127; //Helps with pops/clicks during note change
    } else {
        //Zero voltage
        if(wave == 127) {
            amplitude = 127;
        } else {
            //Positive voltage is 255 -> 128
            if (wave > 127) {
                amplitude = (uint16_t)((wave-127) *  envelope_table[adsr_envelope_level]) >>8;
                amplitude += 128;
            //Negative voltage is 126 -> 0
            } else {
                amplitude = (uint16_t)((127-wave) *  envelope_table[adsr_envelope_level]) >>8;
                amplitude = 126-amplitude;
            }
        }
    }
return amplitude;
}
#endif

}

