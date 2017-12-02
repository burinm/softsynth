#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/*
We are tuning to A at    440.000 Hz
Machine is running at 16000000.000 Hz
Samples are at         22050.000 Hz
Synth timer (samples) will need 363 machine ticks
-------------------------------------------------
*/

/* NON_CORRECTING_TIMING
    Use if not overclocking,
    requires using SAMPLE_DIVIDER to calculate SAMPLE_RATE
*/
#define NON_CORRECTING_TIMING

#define CPU_SPEED   16000000
#define TONE_CLOCK     62500


//#define SAMPLE_DIVIDER  5 // 12500, works but quality suffers
#define SAMPLE_DIVIDER  4  //  15625, fast enough for 4 voice current implementation
//#define SAMPLE_DIVIDER  3 // --- 31250, ideal... but not yet, sample loop goes long
#define SAMPLE_RATE (TONE_CLOCK / SAMPLE_DIVIDER)
//#define SAMPLE_RATE 22050
//#define SAMPLE_RATE 18867 //Experimentally the fastest we can run without timing violations 

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
