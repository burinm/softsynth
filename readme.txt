SoftSynth

This is a software synthesizer that plays multiple 8bit voices and has a 12 bit
output. It supports sin, pulse, triangle, noise and sawtooth waveforms. There is
a simple envelope generator that does Attack, Decay, Sustain and Release. It
supports polyphony up to 8 voices, but is too slow to do this well when also
running the envelope generator.

It takes MIDI input, and basially only reads NOTEON, NOTEOFF messages. It hardcodes
each Voice to a channel.

This soft synthesizer runs in a loop main taking samples as fast as it can.
When it reaches the top, it samples the 16bit Timer1, which is running 62500Hz.
The phase is then calculated from this snapshot in time for all the voices and notes.
For this implementation one phase (2*PI) is 2^16 parts, this way any 16bit value overflow
work perfectly.

In this implementation, overrunning our desired sample rate 22050Hz for each loop
is corrected by reading the current timer. The only time that there is a problem,
is if the loop runs so long the UART buffer is overrun. MIDI is slow, so this isn't
a cause for concern, and interrupts aren't needed to handle the UART. 

There is a sin lookup table of 256 parts, which is mirrored so a sin wave can have
a resolution of 1024 parts.

32bit everything is avoided for speed. Also, we use a phase table, instead of ticks
so that the AVR only needs to multiply, and not divide.


Currently there are 3 ways to compile:

0) Adjust settings in hardware.h

   (Actually, this only matters if using something other than atmel328p 16MHz)

1) Envelopes on, no polyphony

    make -f Makefile.make clean
    make -f Makefile.make
    make -f Makefile.make upload

    This works great with 3 single voices and one drum track.
    Chords are ignored, and the last note is played

2) Envelopes on, polyphony enabled

    Voice.h
        < //#define POLYPHONY
        > #define POLYPHONY

    make -f Makefile.make clean
    make -f Makefile.make
    make -f Makefile.make upload

    This doesn't work very well, fun for playing keyboard solo
    This violates timing a lot, so sometimes notes get stuck
    in the buffer because the NOTEOFF messages are missed (I suspect)

3) Fast Voice mode, with polyphony integrated

    An alternative implementation of the Voice class which
    does not support any kind of envelope generator.

    This works great!

    Drums have a fake "envelope" that uses Timer0 to stop
    the drum sound, so no NOTEOFF needs to be sent.
    
    Pile on the tracks for each voice.

    This compiles in VoiceFast.cpp instead of Voice.cpp

    make -f Makefile.make FASTVOICE=y clean
    make -f Makefile.make FASTVOICE=y 
    make -f Makefile.make FASTVOICE=y upload 
