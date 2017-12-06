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

    #include <stdio.h> //debug only, remove
}

#include "Envelope.h"
#include "NotePoolTiny.cpp"
#include "utils/notes.h"



namespace SoftSynth {

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
        envelope_t adsr_reset;    //Reset values for envelope

        void init(uint8_t (*f)(uint16_t), envelope_t&);

        inline void startNote(uint8_t midinote) {
            #ifdef POLYPHONY
                note_t *temp_note;
                temp_note = note_pool.addValue(midinote);
                if (temp_note) {
//fprintf(stderr,"(+%d)",temp_note->note);
                    temp_note->envelope.start(adsr_reset);
                } else {
fprintf(stderr,"(+bork couldn't add note:%d,0x%x!)",midinote,temp_note);
                }
            #else
                current_note.note = midinote;
                current_note.envelope.start(adsr_reset);
            #endif

            random_reset();
        }

        inline void stopNote(uint8_t midinote) {
            #ifdef POLYPHONY
                note_t *temp_note;
                temp_note = note_pool.getValue(midinote);
                if(temp_note) {
                    temp_note->envelope.setState(ADSR_RELEASE);
//fprintf(stderr,"(!%d)",temp_note->note);
                } else {
//This is o.k, because a note's ADSR can expire before the MIDI_OFF message comes
//fprintf(stderr,"(!bork couldn't find note:%d, 0x%x!)",midinote,temp_note);
                }


            #else
                current_note.envelope.setState(ADSR_RELEASE);
                //should we reset envelope here?
            #endif

        }

        void setWaveform(uint8_t (*wave_function)(uint16_t));
        void setControl(uint8_t control, uint8_t value);

        inline uint16_t sample(uint16_t t) {

                #ifdef POLYPHONY
                    note_t *n_note;
                    uint16_t total_sample=0;


                    while( (n_note = note_pool.getIterate()) != 0 )  {
        //fprintf(stderr,"(p->0x%x)",n_note);

                        if (n_note->envelope.getState() == ADSR_OFF ) {
        //fprintf(stderr,"(-%d)",n_note->note);
                            uint8_t delete_count;
                            if ( (delete_count = note_pool.removeValue(n_note->note)) ==0 ) {
                                fprintf(stderr,"(BORK couldn't remove-%d)",n_note->note);
                            }
        //fprintf(stderr,"(removed n%d,%d)",delete_count,n_note->note);
                            continue; 
                        }
        //fprintf(stderr,"(p%d)",n_note->note);

        //fprintf(stderr,"(t=%d)",t);
                        phase = t * note_phase_mult_table[n_note->note];

                        n_note->envelope.step(adsr_reset);
                        total_sample += (uint8_t)n_note->envelope.apply_envelope(wave_function(phase));
                        //total_sample += wave_function(phase);
                    }
                    return total_sample;
                #else

                                            /*This should be 127,
                                               but causes hiccup in waveform
                                            */
                    if (current_note.envelope.getState() == ADSR_OFF ) {
                     return 0;
                    }
        //fprintf(stderr,"(t=%d)",t);
                    // Automatic 16bit rollover (i.e. phase % PARTS_PER_CYCLE)
                    phase = t * note_phase_mult_table[current_note.note];  //2.4us per voice

                    current_note.envelope.step(adsr_reset);
                    return (uint8_t)current_note.envelope.apply_envelope(wave_function(phase));
                    //return (uint8_t)(wave_function(phase));
                #endif
        }

    private:
        uint16_t phase;


        #ifdef POLYPHONY
            /* Note pool */
            NotePoolTiny note_pool;
            uint8_t note_count;
        #else
            note_t current_note;
        #endif

        uint8_t (*wave_function)(uint16_t);
};


}
#endif
