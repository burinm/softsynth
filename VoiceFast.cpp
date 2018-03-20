/*  Voice.h - Invented Here License v1.0 (2017) - burin

    Voice object for a soft synthesizer 

*/

#include <stdint.h>

#include <stdio.h> //Remove after debug

#include <avr/io.h> //TCNT0
#include <avr/interrupt.h>

extern "C" {
    #include "wave_function.h"
}


#define MAX_VOICES 5
extern const uint16_t note_phase_mult_table[128];

namespace SoftSynth {

#define POOL_MAX    8

//Timer0 counts up from DRUM_DECAY->TOP(0xff)
#define DRUM_DECAY 20 // (256 - 20) * 1/(16MHz/1024) = 15ms

class VoiceFast {
    public:

        void init(uint8_t (*f)(uint16_t)) {
            is_drum = 0;
            drum_on = 0;
            last_note_index = 0;

            wave_function = f;

            for (uint8_t i=0;i<POOL_MAX;i++) {
                note_pool[i] = 0;
            }
        }

        void off() {
            for (uint8_t i=0;i<POOL_MAX;i++) {
                note_pool[i] = 0;
            }
        }

        void setDrum() {
            is_drum = 1;
        }

        inline void setDrumExpire() {
             cli();
                drum_on = 1;
                TCNT0 = DRUM_DECAY;
             sei();
        }

        inline void startNote(uint8_t midinote) {
            if(is_drum) {
                random_reset();
                setDrumExpire();
            }

            addNote(midinote);
        }

        inline void stopNote(uint8_t midinote) {
            removeNote(midinote);
        }

        inline void addNote(uint8_t midinote) {

        #if 1 //Do not add a note twice    
            for (uint8_t i=0;i<POOL_MAX;i++) {
                if (note_pool[i] == midinote) {
                   return; 
                }
            }
        #endif

            for (uint8_t i=0;i<POOL_MAX;i++) {
                if (note_pool[i] == 0) {
                    note_pool[i] = midinote;
                    last_note_index = i;
                    break;
                }
            }
        }

        inline void removeNote(uint8_t midinote) {
            for (uint8_t i=0;i<POOL_MAX;i++) {
                if (note_pool[i] == midinote) {
                    note_pool[i] = 0;
                    break;
                }
            }
        }

        void setWaveform(uint8_t (*f)(uint16_t)) {
             wave_function = f;
        }

        inline uint16_t sample(uint16_t t) {
            static uint16_t phase;
            uint16_t total=0;

            if (is_drum) {
                if (drum_on == 1) {
                    phase = t * note_phase_mult_table[note_pool[last_note_index]];
                    total +=(uint8_t)(wave_function(phase));
                    return total;
                } else {
                    return 0;
                }
            }

            for (uint8_t i=0;i<POOL_MAX;i++) {
                if ( note_pool[i] !=0 ) {
                    phase = t * note_phase_mult_table[note_pool[i]];
                    total +=(uint8_t)(wave_function(phase));
                }
            }
            return total;
        }

        uint8_t is_drum;
        volatile uint8_t drum_on;
        uint8_t last_note_index;
    private:
        uint8_t (*wave_function)(uint16_t);
        uint8_t note_pool[POOL_MAX];
};

}
