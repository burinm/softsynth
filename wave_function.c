/* wave_function.c  - Invented Here License v1.0 (2017) - burin

    Implementation of periodic functions for 8-bit soft synth 

*/

#include "wave_function.h"
#include "hardware.h"

inline uint8_t t_sin(uint16_t t) { //TODO: Add error handling <1024
#define QUADRANT_4  ((WAVE_TABLE_QUANT / 4) * 3)
#define QUADRANT_3  (WAVE_TABLE_QUANT / 2)
#define QUADRANT_2  (WAVE_TABLE_QUANT / 4)
#define QUADRANT_1  0 

    t >>= WT_AMPL_RATIO;

    if (t > QUADRANT_4 - 1) { return                   t_sine_table[MAX_AMPLITUDE - (t - QUADRANT_4) ]  ; }
    if (t > QUADRANT_3 - 1) { return MAX_AMPLITUDE - ( t_sine_table[                (t - QUADRANT_3) ] ); }
    if (t > QUADRANT_2 - 1) { return MAX_AMPLITUDE - ( t_sine_table[MAX_AMPLITUDE - (t - QUADRANT_2) ] ); }
                              return                   t_sine_table[                (t - QUADRANT_1) ]  ;
}

inline uint8_t t_pulse(uint16_t t) {

    //quarter duty cycle
    if (t < ((PARTS_PER_CYCLE / 4) - 1)) { return MAX_AMPLITUDE; }

    return 0;
}

inline uint8_t t_sawtooth(uint16_t t) {
    return t >> BPC_AMPL_RATIO;
}

inline uint8_t t_triangle(uint16_t t) {
    if (t > (QUADRANT_3 - 1)) {
        return (( 2 * ( (PARTS_PER_CYCLE -1) - (t-QUADRANT_3)) ) >> BPC_AMPL_RATIO );
    }

    return ( (2 * t) >> BPC_AMPL_RATIO);
}

inline uint8_t t_noise(uint16_t t) {
static uint16_t previous_1 = 0;
static uint16_t previous_2 = (PARTS_PER_CYCLE -1);
static uint16_t random_number = 9;

 //TODO: investigate previous algorithm
        //Causes notes lower than 0x5 to be too high

    // Lehmer RNG, with Sinclair ZX81 parameters
    if (previous_1 <= previous_2) {
        random_number= ( 75 *random_number) % 65537;
    }

    previous_2 = previous_1;
    previous_1 = t;

    return (uint8_t)(random_number >> 8);
}
