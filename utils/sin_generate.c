/*  sin_generate.c - Invented Here License v1.0 (2017) - burin

    Program to generate a lookup table for SIN function 

*/

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "../hardware.h"


int main() {



uint16_t i_max = (PARTS_PER_CYCLE/4);

printf("/*\n    Quarter sin wave, will mirror the other 3/4\n");
printf("    360 degrees = %d units, this table is 0-%d (1/4)\n",PARTS_PER_CYCLE,i_max-1);
printf("    Max Amplitude %d\n*/\n",MAX_AMPLITUDE);

printf("const uint8_t t_sine_table[%d] = {\n",i_max);

double max_amplitude = MAX_AMPLITUDE;
for (uint16_t i=0;i<i_max;i++) {

    //Quarter sin wave, will mirror the other 3/4
    double phase =  M_PI_2  * ((double)i / (i_max-1));
    double tone = max_amplitude + (max_amplitude * sin(phase + M_PI + M_PI_2));
    uint8_t tone_int = (uint8_t)tone;

    printf("%3d",tone_int);
    if (i<i_max -1) { printf(","); }
    if ( (i+1) % 16 == 0) { printf("\n"); }
}

printf("};\n");
return 0;
}
