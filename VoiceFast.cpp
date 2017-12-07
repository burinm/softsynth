/*  Voice.h - Invented Here License v1.0 (2017) - burin

    Voice object for a soft synthesizer 

*/

#include <stdint.h>

#include <stdio.h> //Remove after debug

#include <avr/io.h> //TCNT0

extern "C" {
    #include "wave_function.h"
}


#define MAX_VOICES 4
extern const uint16_t note_phase_mult_table[128];

namespace SoftSynth {

#define POOL_MAX    8
#define DRUM_DECAY  51 //Timer0 counts up from 51->TOP(0xff)

class VoiceFast {
    public:

        void init(uint8_t (*f)(uint16_t)) {
            is_drum = 0;
            drum_on = 0;

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
                drum_on = 1;
                TCNT0 = 51;
        }

        inline void startNote(uint8_t midinote) {
            if(is_drum) {
                setDrumExpire();
            }

            addNote(midinote);
            random_reset();
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

            if (is_drum && (drum_on == 0)) { return 0; }

            for (uint8_t i=0;i<POOL_MAX;i++) {
                if ( note_pool[i] !=0 ) {
                    phase = t * note_phase_mult_table[note_pool[i]];
                    total +=(uint8_t)(wave_function(phase));
                }
            }
            return total;
        }

        uint8_t is_drum;
        uint8_t drum_on;
    private:
        uint8_t (*wave_function)(uint16_t);
        uint8_t note_pool[POOL_MAX];
};

}
