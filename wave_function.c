/* wave_function.c  - Invented Here License v1.0 (2017) - burin

    Implementation of periodic functions for 8-bit soft synth 

*/

#include "wave_function.h"

uint8_t t_sin(uint16_t t) { //TODO: Add error handling <1024
#define QUADRANT_4  768 
#define QUADRANT_3  512
#define QUADRANT_2  256
#define QUADRANT_1  0 

    if (t > QUADRANT_4 - 1) { return         t_sine_table[255 - (t - QUADRANT_4) ]  ; }
    if (t > QUADRANT_3 - 1) { return 255 - ( t_sine_table[      (t - QUADRANT_3) ] ); }
    if (t > QUADRANT_2 - 1) { return 255 - ( t_sine_table[255 - (t - QUADRANT_2) ] ); }
                              return         t_sine_table[      (t - QUADRANT_1) ]  ;
}

uint8_t t_pulse(uint16_t t) {
    //quarter duty cycle
    if (t < 255) { return 255; }
                  return 0;
}

uint8_t t_sawtooth(uint16_t t) {
    return t >> 2;
}

uint8_t t_triangle(uint16_t t) {
    if (t > 511) {
        return ((1023 - ((2 *(t-512)) >> 2)));
    }

        return (2 * t >> 2);
}

uint8_t t_noise(uint16_t t) { //TODO: incestigate previous algorithm
static uint16_t previous_1 = 0;
static uint16_t previous_2 = 1023;
static uint16_t random_number = 9;

    // Lehmer RNG, with Sinclair ZX81 parameters
    if (previous_1 <= previous_2) {
        random_number= ( 75 *random_number) % 65537;
    }

    previous_2 = previous_1;
    previous_1 = t;

    return (uint8_t)(random_number >> 8);
}
