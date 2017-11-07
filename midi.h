/*  midi.h - Invented Here License v1.0 (2017) - burin

   Simple defines for the MIDI music software protocol 

*/

#ifndef __MIDI_H__
#define __MIDI_H__

#include "Voice.h"
using namespace SoftSynth;

#ifdef __cplusplus
extern "C" {
#endif
   #include "circbuf_tiny.h"
#ifdef __cplusplus
}
#endif

#define MIDI_STATUS_MASK               0x80
#define MIDI_DATA_MASK                 0x7F
#define MIDI_STATUS_TYPE_OFFSET        4 
#define MIDI_STATUS_TYPE_MASK          (0x7 << MIDI_STATUS_TYPE_OFFSET)
#define MIDI_STATUS_CHANNEL_MASK         0xf

#define MIDI_STATUS_NOTE_OFF                0x0
#define MIDI_STATUS_NOTE_ON                 0x1
//#define MIDI_STATUS_POLY_KEY_PRESSURE       0x2
#define MIDI_STATUS_CONTROL                 0x3
#define MIDI_STATUS_PROGRAM_CHANGE          0x4
//#define MIDI_STATUS_CHANNEL_PRESSURE        0x5
#define MIDI_STATUS_PITCH_BEND              0x6
#define MIDI_STATUS_RESET                   0x7

#define MIDI_DEFAULT_CHANNEL    0
#define MIDI_MESSAGE_CHANNEL
#define MIDI_MESSAGE_CHANNEL_VOICE
#define MIDI_MESSAGE_CHANNEL_MODE

#define MIDI_MESSAGE_SYSTEM
#define MIDI_MESSAGE_SYSTEM_EXCLUSIVE
#define MIDI_MESSAGE_SYSTEM_COMMON
#define MIDI_MESSAGE_SYSTEM_RT

/* midi buffer */
extern circbuf_tiny_t midi_buf;

void midi_init();
void process_midi_messages();




#endif
