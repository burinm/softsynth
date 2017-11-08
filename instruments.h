#ifndef __INSTRUMENTS_H__
#define __INSTRUMENTS_H__

#include "Envelope.h"

envelope_t flute_instrument1 = {

    .attack_ticks =     8,
    .attack_count =     127,
    .decay_ticks =      8,
    .decay_count =      20,
    .sustain_ticks =    4096,
    .sustain_hold =     1,
    .release_ticks =    64,
    .release_count =    107,
};

envelope_t flute_instrument2 = {

    .attack_ticks =     1,
    .attack_count =     127,
    .decay_ticks =      0,
    .decay_count =      0,
    .sustain_ticks =    4096,
    .sustain_hold =     1,
    .release_ticks =    16,
    .release_count =    107,
};

envelope_t fatty_base_instrument1 = {

    .attack_ticks =     1,
    .attack_count =     127,
    .decay_ticks =      0,
    .decay_count =      0,
    .sustain_ticks =    8192,
    .sustain_hold =     1,
    .release_ticks =    128,
    .release_count =    107,
};


#endif
