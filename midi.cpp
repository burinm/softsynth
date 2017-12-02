#include "midi.h"
#include <stdlib.h>
#include "Voice.h"
#include "debug.h"


#include <stdio.h> //TODO: remove fprintf debug


using namespace SoftSynth;

/* Voices */
extern Voice voices[];

/* midi buffer */
uint8_t midi_static_alloc[CIRCBUF_TINY_MAX+1];
circbuf_tiny_t midi_buf;

static uint8_t midi_running_status = MIDI_STATUS_NOTE_OFF;
static uint8_t midi_current_channel = MIDI_DEFAULT_CHANNEL;
static uint8_t midi_current_control = 0;

void midi_init() {
    /* initialize midi buffer */
    if (circbuf_tiny_init(&midi_buf,midi_static_alloc) == 0) {
        ERROR_SET(ERROR_FATAL);
    }
}

void process_midi_messages() {
    /* Critial section, but on Atmel328p, interrupt handler is safe */
    static uint8_t byte_out;
    if (CIRCBUF_TINY_SIZE(&midi_buf) > 0)                                            //4.20us
    {
        //Get byte from buffer
        circbuf_tiny_read(&midi_buf, &byte_out);

    #if 1 //Midi processing
        static uint8_t midi_byte_number = 0; 

        if (byte_out & MIDI_STATUS_MASK) {
            midi_byte_number=0;

            midi_running_status=(byte_out & MIDI_STATUS_TYPE_MASK) >> MIDI_STATUS_TYPE_OFFSET;
            midi_current_channel = byte_out & MIDI_STATUS_CHANNEL_MASK;

            if (midi_running_status == MIDI_STATUS_RESET) {
                //TODO: synth.reset();
            }
        } else {

            if (midi_current_channel < MAX_VOICES) {
                switch(midi_running_status) {
                    case    MIDI_STATUS_NOTE_ON:
    //fprintf(stderr,"Note on-->%d",byte_out);
                        if (midi_byte_number == 0) {
                            voices[midi_current_channel].startNote(byte_out);
                            midi_byte_number++;
                            break;
                        }

                        if (midi_byte_number == 1) {
                            if (byte_out == 0 ) { //zero velocity = NOTE_OFF
                                    voices[midi_current_channel].stopNote();
                            }
                            //voice0.velocity = byte_out;
                            midi_byte_number=0;
                            break;
                        }
                        break;
                    case    MIDI_STATUS_NOTE_OFF:
    //fprintf(stderr,"Note off-->0x%x",byte_out);
                                voices[midi_current_channel].stopNote();
                            midi_byte_number=0;
                        break;
                    case    MIDI_STATUS_CONTROL:
                        if (midi_byte_number == 0) {
                            midi_current_control = byte_out;
                            midi_byte_number++;
                            break;
                        }

                        if (midi_byte_number == 1) {
                            voices[midi_current_channel].setControl(midi_current_control,byte_out);
                            midi_byte_number=0;
                            break;
                        }


                        break;
                    default:
                        midi_byte_number = 0;
                        break;
                }
            }
        #endif //end midi processing

        }
    }
    /* end critical */
}
