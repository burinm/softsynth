#include <cstdio>
#include <cstdlib>

extern "C" {
#include "../wave_function.h"
}


int main(int argc, char* argv[]) {

int cycles = 1;

    if(argc == 2) {
        cycles = atoi(argv[1]);
    }

uint32_t counter=0;
uint8_t (*wave_function)(uint16_t);

//wave_function=t_sin;
//wave_function=t_triangle;
//wave_function=t_pulse;
//wave_function=t_sawtooth;
wave_function=t_noise;

    for (; cycles > 0; cycles--) {
        for(uint16_t i=0; (uint16_t)(i+1) > i; i++ ) {
            printf("%d %d\n",counter,wave_function(i));
            counter++;
        }

    }
}
