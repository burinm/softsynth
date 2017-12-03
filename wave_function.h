/* wave_function.h  - Invented Here License v1.0 (2017) - burin

   Functions for calculating periodic functions of sound
    waves. One period (360 debrees) has 1024 units 

    Functions included for
        Sin Pulse Sawtooth Triangle Noise

*/

#ifndef __WAVE_FUNCTION_H__
#define __WAVE_FUNCTION_H__

#include <stdint.h>

uint8_t t_sin(uint16_t t);
uint8_t t_pulse(uint16_t t);
uint8_t t_sawtooth(uint16_t t);
uint8_t t_triangle(uint16_t  t);
uint8_t t_noise(uint16_t  t);

void random_reset();

#endif
