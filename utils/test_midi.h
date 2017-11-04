#ifndef __TEST_MIDI_H__
#define __TEST_MIDI_H__

#include <signal.h>

#define MIDI_INPUT_SIGNAL   (SIGRTMIN)       //Highest priority
#define OSC_CLOCK_SIGNAL    (SIGRTMIN + 1)

void serial_port_setup();
void serial_port_teardown();

#endif
