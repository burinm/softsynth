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

//Midi byte type
#define MIDI_STATUS_BYTE          0x80
#define MIDI_DATA_MASK            0x7F

//Channel status
#define MIDI_STATUS_OFFSET        4
#define MIDI_STATUS_CHANNEL_MASK  0xf
#define MIDI_STATUS_MASK          (MIDI_STATUS_CHANNEL_MASK << MIDI_STATUS_OFFSET)
#define MIDI_GET_STATUS(s)        (((s) & MIDI_STATUS_MASK) >> MIDI_STATUS_OFFSET)

//Channel messages
#define MIDI_STATUS_NOTE_OFF                0x8
#define MIDI_STATUS_NOTE_ON                 0x9
#define MIDI_STATUS_POLY_KEY_PRESSURE       0xa
#define MIDI_STATUS_CONTROL_CHANGE          0xb
#define MIDI_STATUS_PROGRAM_CHANGE          0xc
#define MIDI_STATUS_CHANNEL_PRESSURE        0xd
#define MIDI_STATUS_PITCH_BEND              0xe
#define MIDI_STATUS_SYSTEM                  0xf

#define MIDI_STATUS_NONE                    0xff //Not in specification, implementation specific

#define MIDI_DEFAULT_CHANNEL    0
#define MIDI_MESSAGE_CHANNEL
#define MIDI_MESSAGE_CHANNEL_VOICE
#define MIDI_MESSAGE_CHANNEL_MODE

//System status
#define MIDI_SYSTEM_STATUS_TYPE_BIT         3 
#define MIDI_SYSTEM_STATUS_TYPE_MASK        (1 << MIDI_SYSTEM_STATUS_TYPE_BIT)
#define MIDI_SYSTEM_STATUS_COMMON           0x0
#define MIDI_SYSTEM_STATUS_REALTIME         0x1
#define MIDI_GET_SYSTEM_STATUS_TYPE(s)      ( ((s) & MIDI_SYSTEM_STATUS_TYPE_MASK) >> MIDI_SYSTEM_STATUS_TYPE_BIT)

#define MIDI_SYSTEM_MESSAGE_MASK            0xff
#define MIDI_GET_SYSTEM_MESSAGE(m)          ( (m) & MIDI_SYSTEM_MESSAGE_MASK )

//System common messages
#define MIDI_SYSTEM_MESSAGE_COMMON_MIDI_TCQF        0xf1
#define MIDI_SYSTEM_MESSAGE_COMMON_SONGPOSITION     0xf2
#define MIDI_SYSTEM_MESSAGE_COMMON_SONGSELECT       0xf3
#define MIDI_SYSTEM_MESSAGE_COMMON_TUNEREQUEST      0xf6
#define MIDI_SYSTEM_MESSAGE_COMMON_EOX              0xf7

//System realtime messages
#define MIDI_SYSTEM_MESSAGE_REALTIME_TIMING_CLOCK     0xf8
#define MIDI_SYSTEM_MESSAGE_REALTIME_START            0xfa
#define MIDI_SYSTEM_MESSAGE_REALTIME_CONTINUE         0xfb
#define MIDI_SYSTEM_MESSAGE_REALTIME_ACTIVESENSING    0xfe
#define MIDI_SYSTEM_MESSAGE_REALTIME_SYSTEMRESET      0xff



/* midi buffer */
extern circbuf_tiny_t midi_buf;

void midi_init();
void process_midi_messages();

#endif
