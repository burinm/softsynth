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

/*
    Quarter sin wave, will mirror the other 3/4
    360 degrees = 65536 units, this table is 0-255 (1/4)
    Sin quantization limited to 256 parts, each part = *256
    Max Amplitude 255 only need to encode 1/2 amplitude 127
*/
const uint8_t t_sine_table[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,
  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,
  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14,
 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21,
 21, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28,
 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37,
 37, 38, 38, 39, 39, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56,
 57, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67,
 67, 68, 69, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 76, 77, 78,
 79, 79, 80, 81, 82, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 90,
 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99, 99,100,101,102,
102,103,104,105,106,106,107,108,109,109,110,111,112,113,113,114,
115,116,116,117,118,119,119,120,121,122,123,123,124,125,126,127
};

static uint8_t t_sine_max_table_value=127;



#endif
