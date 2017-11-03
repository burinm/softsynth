#!/bin/bash

rm -rf ./test/*


gcc test_notes.cpp ../Voice.cpp ../Envelope.cpp ../wave_function.c -Wall -ffunction-sections -fdata-sections -Os -pedantic -Wall -Wextra -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -flto -fno-devirtualize -fdiagnostics-color -o ./test/test_notes -lrt
