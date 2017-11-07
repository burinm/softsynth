/* wave_function.c  - Invented Here License v1.0 (2017) - burin

    Implementation of periodic functions for 8-bit soft synth 

*/

#include "wave_function.h"
#include "hardware.h"

inline uint8_t t_sin(uint16_t t) { //TODO: Add error handling <1024
#define QUADRANT_4  (WAVE_TABLE_QUANT * 3)
#define QUADRANT_3  (WAVE_TABLE_QUANT * 2)
#define QUADRANT_2  (WAVE_TABLE_QUANT)
#define QUADRANT_1  0 

    t >>= WT_AMPL_RATIO;

    if (t > QUADRANT_4 - 1) { return                   t_sine_table[MAX_AMPLITUDE - (t - QUADRANT_4) ]  ; }
    if (t > QUADRANT_3 - 1) { return MAX_AMPLITUDE - ( t_sine_table[                (t - QUADRANT_3) ] ); }
    if (t > QUADRANT_2 - 1) { return MAX_AMPLITUDE - ( t_sine_table[MAX_AMPLITUDE - (t - QUADRANT_2) ] ); }
                              return                   t_sine_table[                (t - QUADRANT_1) ]  ;
}

inline uint8_t t_pulse(uint16_t t) {

    //quarter duty cycle (-2) = divide by 2
    if (t < (( 1 << (BITS_PER_PART_PER_CYCLE - 2)) - 1)) { return MAX_AMPLITUDE; }

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

static uint16_t previous_1 = 0;
static uint16_t random_number = 9;

inline uint8_t t_noise(uint16_t t) {

 //TODO: investigate previous algorithm, currently broken

    // Lehmer RNG, with Sinclair ZX81 parameters
    if (t < previous_1) {
        random_number= ( 75 *random_number) % 65537;
    }

    previous_1 = t;

    return (uint8_t)(random_number >> 8);
}

inline void random_reset() {
    previous_1 = 0;
}
