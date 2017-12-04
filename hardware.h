#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/*
We are tuning to A at    440.000 Hz
Machine is running at 16000000.000 Hz
Samples are at         22050.000 Hz
Synth timer (samples) will need 363 machine ticks
-------------------------------------------------
*/
//#define OVERCLOCK


#define CPU_SPEED           16000000
#define SAMPLE_TIMER_DIV    8


#ifdef OVERCLOCK
    #define CORRECTIVE_TIMING
    #define SAMPLE_RATE 31250
    #define TIMER_DIV   1
#else
//#define SAMPLE_RATE 17000
#define SAMPLE_RATE 15625
//#define SAMPLE_RATE 22050
//#define SAMPLE_RATE 18867 //Experimentally the fastest we can run without timing violations 
#endif


#define BITS_PER_PART_PER_CYCLE 16 
#define PARTS_PER_CYCLE         (1 << BITS_PER_PART_PER_CYCLE) //currently 2^16
#define PARTS_PER_CYCLE_HALF    (uint16_t)(1 << (BITS_PER_PART_PER_CYCLE - 1)) // 2^15
#define PARTS_PER_CYCLE_QUARTER (uint16_t)(1 << (BITS_PER_PART_PER_CYCLE - 2)) // 2^14

#define BITS_AMPLITUDE          8
#define MAX_AMPLITUDE           (uint8_t)((1 << BITS_AMPLITUDE) -1)     //currently 255
#define HALF_AMPLITUDE          (uint8_t)((1 << BITS_AMPLITUDE) /2)     //currently 128

#define BPC_AMPL_RATIO          (uint8_t)(BITS_PER_PART_PER_CYCLE - BITS_AMPLITUDE)

#define BITS_WAVE_TABLE_QUANT   8 
#define WAVE_TABLE_QUANT        (1 << BITS_WAVE_TABLE_QUANT)
#define BITS_WAVE_TABLE_MIRROR  2 //wave table is 1/4 of total table
#define WT_AMPL_RATIO           (BITS_PER_PART_PER_CYCLE - BITS_WAVE_TABLE_QUANT - BITS_WAVE_TABLE_MIRROR)


#endif
