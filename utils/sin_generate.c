/*  sin_generate.c - Invented Here License v1.0 (2017) - burin

    Program to generate a lookup table for SIN function 

*/

#include <stdint.h>
#include <stdio.h>
#include <math.h>

int main() {

printf("const uint8_t t_sine_table[256] = {\n");

for (int i=0;i<256;i++) {

    //Quarter sin wave, will mirror the other 3/4
    double phase =  M_PI_2  * ((double)i / 255);
    double tone = 127.0 + (127.0 * sin(phase + M_PI + M_PI_2));
    uint8_t tone_int = (uint8_t)tone;

    printf("%3d",tone_int);
    if (i<255) { printf(","); }
    if ( (i+1) % 16 == 0) { printf("\n"); }
}

printf("};\n");
return 0;
}
