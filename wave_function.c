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
    if (t < PARTS_PER_CYCLE_QUARTER) { return MAX_AMPLITUDE; }

    return 0;
}

inline uint8_t t_sawtooth(uint16_t t) {
    return t >> BPC_AMPL_RATIO;
}

inline uint8_t t_triangle(uint16_t t) {
    if (t > PARTS_PER_CYCLE_HALF) {
        return ( (2 * ( PARTS_PER_CYCLE - t )) >> BPC_AMPL_RATIO );
    }

    return ( (2 * t) >> BPC_AMPL_RATIO);
}

static uint16_t previous_1 = 0;
static uint16_t random_number = 4;
static uint16_t tmp_random_number;

inline uint8_t t_noise(uint16_t t) {

    // Lehmer RNG, with small parameters that fit in 2^16 int
    if (t < previous_1) {
        //random_number= (uint16_t)( 74 *random_number) % (uint16_t)32771;
#if 1 //Doesn't profile as any faster
        // 32771 = 2^15 + 3
        random_number = (uint16_t)( 74 *random_number);
        tmp_random_number = random_number>>15;
        if (tmp_random_number) {
            tmp_random_number = random_number - (1<<15);
            if (tmp_random_number>3)
                random_number = random_number - ((1<<15) + 3);
        }
#endif
    }

    previous_1 = t;

    return (uint8_t)(random_number & 0xff);
}

inline void random_reset() {
    previous_1 = 0;
}
