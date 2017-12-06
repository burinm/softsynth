#!/bin/bash

rm -rf ./test/*

# -Os

gcc -g -D_GNU_SOURCE test_notes.cpp test_midi.cpp ../Voice.cpp ../Envelope.cpp ../wave_function.c ../midi.cpp ../circbuf_tiny.c ../NotePoolTiny.cpp ../voice_notes.c -Wall -ffunction-sections -fdata-sections -Wall -Wextra -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color -o ./test/test_notes -lrt -lm

gcc -g -D_GNU_SOURCE -DFASTVOICE test_notes.cpp test_midi.cpp ../VoiceFast.cpp ../Envelope.cpp ../wave_function.c ../midi.cpp ../circbuf_tiny.c ../NotePoolTiny.cpp ../voice_notes.c -Wall -ffunction-sections -fdata-sections -Wall -Wextra -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color -o ./test/test_notes_fast -lrt -lm
