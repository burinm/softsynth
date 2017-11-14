#include <cstdio>
#include <cstdlib>

#include "../Envelope.h"
using namespace SoftSynth;

extern "C" {
#include "../wave_function.h"
}

envelope_t test_instrument = {

    .attack_ticks =     64,
    .attack_count =     127,
    .decay_ticks =      64,
    .decay_count =      20,
    .sustain_ticks =    4096,
    .sustain_hold =     1,
    .release_ticks =    64,
    .release_count =    107,
};


int main(int argc, char* argv[]) {

int cycles = 1;

    if(argc == 2) {
        cycles = atoi(argv[1]);
    }

uint16_t amplitude=0;
uint32_t counter=0;
uint8_t (*wave_function)(uint16_t);
Envelope e;
e.init(test_instrument);
e.start();
e.test_set_adsr_envelope_level(127);

//wave_function=t_sin;
wave_function=t_triangle;
//wave_function=t_pulse;
//wave_function=t_sawtooth;
//wave_function=t_noise;


    for (; cycles > 0; cycles--) {
        for(uint16_t i=0; (uint16_t)(i+1) > i; i++ ) {
        //for(uint16_t i=0; i<17000; i++ ) {
        //for(uint16_t i=0; i<256; i++ ) {
            //e.step();
            amplitude = e.apply_envelope(wave_function(i));
            //printf("%d %d\n",counter,wave_function(i));
            printf("%d %d\n",counter,amplitude);
            //printf("%d %d %d\n",counter,wave_function(i),amplitude);
            //printf("%d %d\n",i,e.apply_envelope(i));
            counter++;
        }

    }
}
