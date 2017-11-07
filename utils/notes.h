#ifndef __NOTES_H__
#define __NOTES_H__

/* Create chart of notes

C-1 = midi note 0
A4  = midi note 69 
G9 =  midi not  127

               [88 key piano]
    A0 (midi 21)     <-->   C8 (midi 108)

               [casio keyboard]
        C1 (midi 24) <--> C6 (midi 84)
*/

#define STEPS_IN_OCTAVE 12 
#define HALF_STEPS_FROM_C   9
#define OFFSET_A4_NOTE  ((4 * STEPS_IN_OCTAVE)  + HALF_STEPS_FROM_C)
#define MIDI_A4_NOTE   69 
#endif
