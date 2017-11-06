/*  sin_generate.c - Invented Here License v1.0 (2017) - burin

    Program to generate a lookup table for SIN function 

*/

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "../hardware.h"


int main() {

printf("/*\n    Quarter sin wave, will mirror the other 3/4\n");
printf("    360 degrees = 1024 units, this table is 0-255 (1/4)\n");
printf("    Max Amplitude 127\n*/\n");


uint16_t i_max = (PARTS_PER_CYCLE/4);

printf("const uint8_t t_sine_table[%d] = {\n",i_max);

for (uint16_t i=0;i<i_max;i++) {

    //Quarter sin wave, will mirror the other 3/4
    double phase =  M_PI_2  * ((double)i / (i_max-1));
    double tone = 127.0 + (127.0 * sin(phase + M_PI + M_PI_2));
    uint8_t tone_int = (uint8_t)tone;

    printf("%3d",tone_int);
    if (i<i_max -1) { printf(","); }
    if ( (i+1) % 16 == 0) { printf("\n"); }
}

printf("};\n");
return 0;
}
