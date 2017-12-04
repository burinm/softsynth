/*  Voice.h - Invented Here License v1.0 (2017) - burin

    Voice object for a soft synthesizer 

*/

#ifndef __VOICE_H__
#define __VOICE_H__

#define MAX_VOICES 4
//#define POLYPHONY

extern "C" {
    #include <stdint.h> //avr-g++ doesn't support <cstdint>??
    #include "wave_function.h" //TODO: for random_reset, find better way to encapsulate this
    #include "pool_tiny.h"
}

#include "Envelope.h"
#include "utils/notes.h"



namespace SoftSynth {

//Current note bit 7 is gate, bits 0-6 are note
#define NOTE_GATE_BIT   7
#define NOTE_GATE_MASK  (1 << NOTE_GATE_BIT)
#define NOTE_OFF(u8) ((u8) &= ~NOTE_GATE_MASK)
#define NOTE_ON(u8)  ((u8) |= NOTE_GATE_MASK)
#define IS_NOTE_ON(u8)  ((u8) & NOTE_GATE_MASK)
#define GET_NOTE(u8)    (u8) & ~NOTE_GATE_MASK
//Setting note also turns on gate
#define SET_NOTE(u8,note)    ((u8) = (note | (1<<NOTE_GATE_BIT)))

enum VOICE_CONTROLS {
                      ATTACK_COUNT_CTRL = 14,
                      DECAY_COUNT_CTRL = 15,
                      SUSTAIN_COUNT_CTRL = 16,
                      RELEASE_COUNT_CTRL = 17,

                      ATTACK_TICKS_CTRL = 18,
                      DECAY_TICKS_CTRL = 19,
                      SUSTAIN_TICKS_CTRL = 20,
                      RELEASE_TICKS_CTRL = 21 
                    };

class Voice {
    public:
        static const uint16_t note_phase_mult_table[128];

        void init(uint8_t (*f)(uint16_t), envelope_t&);

        inline void startNote(uint8_t midinote) {
            #ifdef POLYPHONY
                if (pool_tiny_add(&note_pool,midinote)) {

                    envelope.start();

#if 0
                    //Restart envelope at beginning of phrase, or if adding
                    // more notes to decaying envelope

                    if (envelope.getState() == ADSR_OFF) {
                    } else {
                        if (envelope.getState() >ADSR_SUSTAIN) {
                            envelope.setState(ADSR_SUSTAIN);
                        }
                    }
#endif
                } //else, too many notes, can't add
            #else
                SET_NOTE(current_note,midinote);
                envelope.start();
            #endif

            random_reset();
        }

        inline void stopNote(uint8_t midinote) {
            #ifdef POLYPHONY
                pool_tiny_remove(&note_pool,midinote);
                //TODO: polyphony count - only release when empty
                if (note_pool.count == 0) {
                    envelope.setState(ADSR_RELEASE);
                }
            #else
                //NOTE_OFF(current_note);
                //NOTE_OFF(midinote);
                envelope.setState(ADSR_RELEASE);
                //should we reset envelope here?
            #endif

        }

        void setWaveform(uint8_t (*wave_function)(uint16_t));
        void setControl(uint8_t control, uint8_t value);

        inline uint16_t sample(uint16_t t) {
                    envelope.step();

                                                    /*This should be 127,
                                                       but causes hiccup in waveform
                                                    */
                   if (envelope.getState() == ADSR_OFF ) { return 0; }

                #ifdef POLYPHONY
                    uint8_t n_note;
                    uint16_t total_sample=0;
                    while(pool_tiny_get_next(&note_pool,&n_note)) {
                        if ( POOL_DATA_VALID(n_note) ) {
                            phase = t * note_phase_mult_table[POOL_ITEM(n_note)];

                            //TODO: Probably should apply envelope at the end - save cycles
                            total_sample += envelope.apply_envelope(wave_function(phase));
                            //total_sample += wave_function(phase);
                        }
                    }
                    return total_sample;
                #else
                    // Automatic 16bit rollover (i.e. phase % PARTS_PER_CYCLE)
                    phase = t * note_phase_mult_table[GET_NOTE(current_note)];  //2.4us per voice

                    return (uint8_t)envelope.apply_envelope(wave_function(phase));
                    //return (uint8_t)(wave_function(phase));
                #endif
        }

    private:
        uint16_t phase;


        #ifdef POLYPHONY
            /* Note pool */
            tiny_pool_t note_pool;
        #else
            uint8_t current_note;
        #endif

        uint8_t (*wave_function)(uint16_t);
        Envelope envelope;
};


}
#endif
