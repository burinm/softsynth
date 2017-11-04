#!/bin/bash

rm -rf ./test/*

# -Os

gcc -g -D_GNU_SOURCE test_notes.cpp test_midi.c ../Voice.cpp ../Envelope.cpp ../wave_function.c ../midi.cpp ../circbuf_tiny.c -Wall -ffunction-sections -fdata-sections -pedantic -Wall -Wextra -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color -o ./test/test_notes -lrt
