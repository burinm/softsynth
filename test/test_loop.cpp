/*  test_loop.cpp - Invented Here License v1.0 (2017) - burin

    Test Harness code
    8-bit soft synth on Arduino Atmega328p

*/

#include <cstdio>

#include "../Voice.h"
#include "../Envelope.h"
#include "../wave_function.h"

using namespace SoftSynth;

Voice voice0;
envelope_t flute_instrument = {

    .attack_ticks =     16,
    .attack_count =     127,
    .decay_ticks =      16,
    .decay_count =      20,
    .sustain_ticks =    4096,
    .sustain_hold =     1,
    .release_ticks =    16,
    .release_count =    107,
};

int main() {

     voice0.init(t_sin, flute_instrument);


    for (;;) {
     static uint16_t synth_clock=0;
        synth_clock++;
        voice0.step();
        uint8_t sample;
        sample = voice0.sample(synth_clock);

    }
}
