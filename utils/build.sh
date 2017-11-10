#!/bin/bash
rm -f ./bin/*
gcc dithering.c -lm -o ./bin/dither
gcc envelope_curve.c -lm -o ./bin/envelope
gcc notes.c -lm -o ./bin/notes
gcc sin_generate.c -lm -o ./bin/sin
gcc test_wave.cpp ../wave_function.c ../Voice.cpp ../Envelope.cpp -o ./bin/wave
