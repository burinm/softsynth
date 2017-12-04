#!/bin/bash
rm -f ./bin/*
gcc -Wall dithering.c -lm -o ./bin/dither
gcc -Wall envelope_curve.c -lm -o ./bin/envelope
gcc -Wall notes.c -lm -o ./bin/notes
gcc -Wall sin_generate.c -lm -o ./bin/sin
gcc -Wall test_wave.cpp ../wave_function.c ../Voice.cpp ../pool_tiny.c ../Envelope.cpp -o ./bin/wave
