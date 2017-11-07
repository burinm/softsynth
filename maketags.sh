#!/bin/bash

ARDUINO_SDK=/home/burin/arduino

find -iname "*.h" -or -iname "*.c" -or -iname "*.cpp" -or -iname "*.s" -or -name Makfile > cscope.files

find ${ARDUINO_SDK}/hardware/arduino/avr/cores/arduino/ -iname "*.h" -or -iname "*.c" -or -iname "*.cpp" -or -iname "*.s" -or -name Makfile >> cscope.files
find ${ARDUINO_SDK}/hardware/arduino/avr/variants/standard/ -iname "*.h" -or -iname "*.c" -or -iname "*.cpp" -or -iname "*.s" -or -name Makfile >> cscope.files

find ${ARDUINO_SDK}/hardware/tools/avr/avr/include/avr/ -iname "*.h" ! -iname "io*.h" >> cscope.files
find ${ARDUINO_SDK}/hardware/tools/avr/avr/include/avr/io.h >> cscope.files
find ${ARDUINO_SDK}/hardware/tools/avr/avr/include/avr/iom328p.h >> cscope.files

cscope -b
ctags -L ./cscope.files
cscope
