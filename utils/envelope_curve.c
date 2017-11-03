/*  envelope_curve.c - Invented Here License v1.0 (2017) - burin

    Program to generate an exponential lookup table
    for a soft synth envelope generator 

*/

#include <stdint.h>
#include <stdio.h>
#include <math.h>

int main() {

printf("/*\n    Envelope Table, pow(.965,i) * 2^8\n*/\n");
printf("const uint8_t t_envelope_table[128] = {\n");

//for (int a=0;a<128;a++) {
for (int i=127;i>=0;i--) {

    // 256 data points, also needs 32bits...
    //double pows= pow(.98,i); //This worked well for 256 data points 
    //envelope_1 *= 8192; // 2^13
    //printf("%d ",((uint32_t)128 * envelope_int) >> 13);

    double pows=  pow(.965,i);
    double envelope_1 =  pows;
    //envelope_1 *=4096; //2^12;
    envelope_1 *=255; //2^8;

    uint16_t envelope_int=(uint16_t)envelope_1;

    // This is how we undo the function, amplitude a
    //printf("%d %d \n",i, ((uint16_t)128 * envelope_int>>12) );

#if 1
    if (i == 127) { envelope_int = 0; } //Make fisrt entry of envelope silent
    printf("%5d",envelope_int);
    if (i>0) { printf(","); }
    if ( (i) % 8 == 0) { printf("\n"); }
#endif

    //printf("%d %d\n",i,envelope_int);
//    printf("%d \n",envelope_int);

#if 1


#endif
}
//printf("\n");
//}

printf("};\n");
return 0;
}
