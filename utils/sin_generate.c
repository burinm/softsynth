/*  sin_generate.c - Invented Here License v1.0 (2017) - burin

    Program to generate a lookup table for SIN function 

*/

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "../hardware.h"


int main() {



uint16_t quant_part = PARTS_PER_CYCLE/WAVE_TABLE_QUANT;
double half_amplitude = MAX_AMPLITUDE/2;

printf("/*\n    Quarter sin wave, will mirror the other 3/4\n");
printf("    360 degrees = %d units, this table is 0-%d (1/4)\n",PARTS_PER_CYCLE,WAVE_TABLE_QUANT-1);
printf("    Sin quantization limited to %d parts, each part = *%d\n", WAVE_TABLE_QUANT,quant_part);
printf("    Max Amplitude %d only need to encode 1/2 amplitude %d\n*/\n",MAX_AMPLITUDE,(uint16_t)half_amplitude);

printf("const uint8_t t_sine_table[%d] = {\n",WAVE_TABLE_QUANT);

for (uint16_t i=0;i<WAVE_TABLE_QUANT;i++) {

    //Quarter sin wave, will mirror the other 3/4
    double phase =  M_PI_2  * ((double)((i+1) * quant_part) / (PARTS_PER_CYCLE-1));
    double tone = half_amplitude + (half_amplitude * sin(phase + M_PI + M_PI_2));
    uint8_t tone_int = (uint8_t)tone;

    printf("%3d",tone_int);
    if (i<WAVE_TABLE_QUANT -1) { printf(","); }
    if ( (i+1) % 16 == 0) { printf("\n"); }

}

printf("};\n");

return 0;
}
