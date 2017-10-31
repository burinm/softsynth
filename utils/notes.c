/*  notes.c - Invented Here License v1.0 (2017) - burin

    Program to generate a lookup table for MIDI notes
    for a soft synth envelope generator 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STEPS_IN_OCTAVE 12 
#define OFFSET_A4_NOTE  (4 * STEPS_IN_OCTAVE)
#define MIDI_A4_NOTE    57

static const char* note_names[12] = { "A", "Bb", "B", "C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#"};

int main(int argc, char* argv[]) {
int table_mode = 0;


/* Machine timing */
const double machine_frequency  = 16E6;
const double sample_frequency   = 44.1E3;

double machine_period = 1 / machine_frequency;
double timer_ticks_per_sample = machine_frequency / sample_frequency; 
unsigned int timer_ticks_per_sample_int = (unsigned int)timer_ticks_per_sample + 1;


double timer_period = timer_ticks_per_sample_int * machine_period;


/* Frequency of a musical note is calculated with this formula:

    f = note_num * [2^(1/12)] ^ n

    Where n is the offset +/- from note_num in half steps
    Where note_num is chosen as an arbitrary starting point
        for note_num = 0 , this program is using  A4
*/

const double twelfth_root_of_2 = 1.05946309436;
double tuning_a = 440;

if (argc > 1) {
    tuning_a = atof(argv[1]);
}

if (argc > 2) {
    if (strcmp("-t", argv[2]) == 0) {
        table_mode=1;
    }
}
    

if (table_mode) { printf("/*\n"); }

printf("We are tuning to A at %10.3f Hz\n", tuning_a);
printf("Machine is running at %10.3f Hz\n", machine_frequency);
printf("Samples are at        %10.3f Hz\n", sample_frequency);
printf("Synth timer (samples) will need %d machine ticks\n",(unsigned int)timer_ticks_per_sample_int);
printf("-------------------------------------------------\n");

if (table_mode) { printf("*/\n\n"); }

if (table_mode) { printf("const uint16_t note_phase_mult_table[128] = {\n    "); }


int octave;
for (octave=-1;octave<10;octave++) {
    int steps;

    for (steps=0;steps<STEPS_IN_OCTAVE;steps++) { //Starting with "A"
        double frequency;
        double ticks;
        double phase_multiplier;
        unsigned int phase_multiplier_int;
        unsigned int ticks_int;
        int note_num; // A 440Hz is note 0
        int midi_note;

        note_num = (octave * STEPS_IN_OCTAVE) +steps - OFFSET_A4_NOTE; 
        midi_note = note_num + MIDI_A4_NOTE; 
        if (midi_note >=0 && midi_note < 128) { //We are iterating A-F, midi octave starts with C
            frequency = tuning_a * pow(twelfth_root_of_2,note_num);
            ticks = 1 / (frequency * timer_period); 
            ticks_int = (unsigned int)ticks;

            //Phase is 1024 degrees, multiply by 64 to just fit in 2^16 int
            // To find phase, tick_number * phase_multiplier[note] >> 6
            phase_multiplier = round((1024 * 64)/ticks);
            phase_multiplier_int = (unsigned int)phase_multiplier;

            if (table_mode) {
                
                //printf("%5d",ticks_int);
                printf("%6d",phase_multiplier_int);

                if (midi_note < 127) { printf(","); }
                if ((midi_note +1) % 12 == 0) { printf("\n    "); }

            } else {
                printf("[midi %3d] note %3d (%-2s):",midi_note, note_num, note_names[steps]);
                printf(" %10.3f Hz synth ticks:%5d %7d\n", frequency, ticks_int, phase_multiplier_int);
            }
        }
    }
}

if (table_mode) { printf("\n};\n"); }

return 0;
}
