#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/*
We are tuning to A at    440.000 Hz
Machine is running at 16000000.000 Hz
Samples are at         22050.000 Hz
Synth timer (samples) will need 363 machine ticks
-------------------------------------------------
*/

#define CPU_SPEED   16000000
#define TONE_CLOCK     62500
//#define SAMPLE_RATE 22050
#define SAMPLE_RATE 14285
//#define SAMPLE_RATE 11025
//#define SAMPLE_RATE 5512

#define BITS_PER_PART_PER_CYCLE 16
#define PARTS_PER_CYCLE         (1 << BITS_PER_PART_PER_CYCLE) //currently 4096 

#define BITS_AMPLITUDE          8
#define MAX_AMPLITUDE           ((1 << BITS_AMPLITUDE) -1)     //currently 255
#define HALF_AMPLITUDE          ((1 << BITS_AMPLITUDE) /2)     //currently 128

#define BPC_AMPL_RATIO          (BITS_PER_PART_PER_CYCLE - BITS_AMPLITUDE)

#define BITS_WAVE_TABLE_QUANT   8 
#define WAVE_TABLE_QUANT        (1 << BITS_WAVE_TABLE_QUANT) 
#define WAVE_TABLE_MIRROR_BITS  (-2) //wave table is 1/4 of total table
#define WT_AMPL_RATIO           (BITS_PER_PART_PER_CYCLE - BITS_WAVE_TABLE_QUANT + WAVE_TABLE_MIRROR_BITS)


#endif
