# softsynth

An 8-bit synthesizer based on the atmega328p AVR

## Features:
* Basic MIDI - note on/off.
* Voices implemented as objects. Instantiate as many as the AVR speed will allow.
* Polyphony support. Note pool currently hardcoded as 8 notes.
* Envelope generator (attack/decay/sustain/release).
* Sine, Pulse, Sawtooth, Triangle and Noise waveforms.
## Options
* Polyphony enable/disable
* Envelope enable/disable - because we are at the limit of CPU resources.

![Output waveform](sounds.png?raw=true "Sound output on oscilloscope")
