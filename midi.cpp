#include "midi.h"
#include <stdlib.h>

#include "debug.h"


#include <stdio.h> //TODO: remove fprintf debug


#ifdef FASTVOICE
    #include "VoiceFast.cpp"
#else
    #include "Voice.h"
#endif

using namespace SoftSynth;

/* Voices */
#ifdef FASTVOICE
    extern VoiceFast voices[];
#else
    extern Voice voices[];
#endif

/* midi buffer */
uint8_t midi_static_alloc[CIRCBUF_TINY_MAX+1];
circbuf_tiny_t midi_buf;

static uint8_t midi_running_status = MIDI_STATUS_NONE;
static uint8_t midi_current_channel = MIDI_DEFAULT_CHANNEL;
static uint8_t midi_current_control = 0;
static uint8_t midi_current_note = 0;

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
//fprintf(stderr,"[0x%x]",byte_out);

    #if 1 //Midi processing
        static uint8_t midi_byte_number = 0; 

        if (byte_out & MIDI_STATUS_BYTE) { //Status Byte
            if (MIDI_GET_STATUS(byte_out) < MIDI_STATUS_SYSTEM) {
                midi_byte_number=0;
                midi_running_status=(byte_out & MIDI_STATUS_MASK) >> MIDI_STATUS_OFFSET;
                midi_current_channel = byte_out & MIDI_STATUS_CHANNEL_MASK;
            } else { // System Message

                        //REALTIME, do nothing to status
                if (MIDI_GET_SYSTEM_STATUS_TYPE(byte_out) == MIDI_SYSTEM_STATUS_REALTIME) {
                    switch(MIDI_GET_SYSTEM_MESSAGE(byte_out)) {
                        case MIDI_SYSTEM_MESSAGE_REALTIME_SYSTEMRESET:
                            //TODO: synth.reset();
                            //fprintf(stderr,"[RESET]");
                            break;
                        default:
                            //fprintf(stderr,"[Unhandled system realtime %d]",MIDI_GET_SYSTEM_MESSAGE(byte_out));

                            break;
                    }

                } else { //COMMON, reset running status
                            //fprintf(stderr,"[Unhandled system common %d]",MIDI_GET_SYSTEM_MESSAGE(byte_out));
                    midi_running_status=MIDI_STATUS_NONE;
                    midi_byte_number=0;
                }
            }

        } else {                            //Data byte

            if (midi_current_channel < MAX_VOICES) {
                switch(midi_running_status) {
                    case    MIDI_STATUS_NOTE_ON:
                        if (midi_byte_number == 0) {
                            midi_current_note = byte_out;
                            midi_byte_number++;
                            break;
                        }

                        if (midi_byte_number == 1) {
                            if (byte_out == 0 ) { //zero velocity = NOTE_OFF
    //fprintf(stderr,"Note off velocity-->%d",midi_current_note);
                                voices[midi_current_channel].stopNote(midi_current_note);
                            } else {
    //fprintf(stderr,"(Note on-->%d)",midi_current_note);
                                voices[midi_current_channel].startNote(midi_current_note);
                            }
                            //voice0.velocity = byte_out;
                            midi_byte_number=0;
                            break;
                        }
                        break;
                    case    MIDI_STATUS_NOTE_OFF:
                        if (midi_byte_number == 0) {
                            midi_current_note = byte_out;
                            midi_byte_number++;
                            break;
                        }

                        if (midi_byte_number == 1) {
    //fprintf(stderr,"(Note off-->%d)",midi_current_note);
                            voices[midi_current_channel].stopNote(midi_current_note);
                            //Release velocity unused
                            midi_byte_number=0;
                        }
                        break;

                    case    MIDI_STATUS_CONTROL_CHANGE:
                        if (midi_byte_number == 0) {
                            midi_current_control = byte_out;
                            midi_byte_number++;
                            break;
                        }

                        if (midi_byte_number == 1) {

                            switch (midi_current_control) {
                                case MIDI_STATUS_CONTROL_MODE_ALL_NOTES_OFF:
                                    if (byte_out == MIDI_STATUS_CONTROL_MODE_ALL_NOTES_OFF_V) {
                                       //voices[midi_current_channel].off();
                                    }
                                    break;
                                default:

                                #ifndef FASTVOICE
                                    voices[midi_current_channel].setControl(midi_current_control,byte_out);
                                #endif
                                    break;
                            };
                            midi_byte_number=0;
                        }
                        break;

                    default:
                        //fprintf(stderr,"[Unhandled running status!!-->0x%x]",midi_running_status);
                        midi_byte_number = 0;
                        break;
                }
            }
        #endif //end midi processing

        }
    }
    /* end critical */
}
