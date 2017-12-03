/* wave_function.c  - Invented Here License v1.0 (2017) - burin

    Implementation of periodic functions for 8-bit soft synth 

*/

#include "wave_function.h"
#include "sin_table.h"
#include "hardware.h"

uint8_t t_sin_quadrant;
uint8_t t_sin_lookup;
inline uint8_t t_sin(uint16_t t) {

/*
    Quadrants are 1/4 of 2^10 i.e. BITS_WAVE_TABLE_QUANT + WAVE_TABLE_MIRROR_BITS
     Right shift additional 8 bits to fit MSBit in uint8_t
*/
#define QUADRANT_4  0x3     // 768 >>8
#define QUADRANT_3  0x2     // 512 >>8
#define QUADRANT_2  0x1     // 256 >>8
#define QUADRANT_1  0x0

   t >>= WT_AMPL_RATIO;


#if 0
    if (t > QUADRANT_4 - 1) { return                   t_sine_table[MAX_AMPLITUDE - (t - QUADRANT_4) ]  ; }
    if (t > QUADRANT_3 - 1) { return MAX_AMPLITUDE - ( t_sine_table[                (t - QUADRANT_3) ] ); }
    if (t > QUADRANT_2 - 1) { return MAX_AMPLITUDE - ( t_sine_table[MAX_AMPLITUDE - (t - QUADRANT_2) ] ); }
                              return                   t_sine_table[                (t - QUADRANT_1) ]  ;
#endif
    t_sin_quadrant = (uint8_t)(t >> 8);
    //t_sin_lookup = (uint8_t)(t & 0xff); //which is faster
    t_sin_lookup = (uint8_t)t;

    switch (t_sin_quadrant) {
        case QUADRANT_4:
            return                        t_sine_table[MAX_AMPLITUDE - t_sin_lookup];
        case QUADRANT_3:
            return       MAX_AMPLITUDE - (t_sine_table[                t_sin_lookup]);
        case QUADRANT_2:
            return       MAX_AMPLITUDE - (t_sine_table[MAX_AMPLITUDE - t_sin_lookup]);
        default:
            return                        t_sine_table[                t_sin_lookup];
    }
}

uint16_t even_out = 0;
uint8_t high_count=0;
uint8_t last_high_count=0;
inline uint8_t t_pulse(uint16_t t) {

    //quarter duty cycle
    if (t < PARTS_PER_CYCLE_QUARTER) { return MAX_AMPLITUDE; }

    return 0;
}

inline uint8_t t_sawtooth(uint16_t t) {
    return (t >> BPC_AMPL_RATIO);
}

inline uint8_t t_triangle(uint16_t t) {
    if (t > PARTS_PER_CYCLE_HALF) {
                        //Depends on t being 2^16 uint
        return ( (2 * ( - t )) >> BPC_AMPL_RATIO );
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
