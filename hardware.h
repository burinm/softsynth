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
//#define SAMPLE_RATE 22050
#define SAMPLE_RATE 11025
//#define SAMPLE_RATE 8000

#define BITS_PER_PART_PER_CYCLE 10
#define PARTS_PER_CYCLE         (1 << BITS_PER_PART_PER_CYCLE) //currently 1024

#define BITS_AMPLITUDE          8
#define MAX_AMPLITUDE           ((1 << BITS_AMPLITUDE) -1)     //currently 255

#define BPC_AMPL_RATIO          (BITS_PER_PART_PER_CYCLE - BITS_AMPLITUDE)


#endif
