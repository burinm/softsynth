
AVR_TOOLS_PATH := /opt/avr-tools
CC := avr-gcc
CXX := avr-g++
SIZE := avr-size
OBJCOPY := avr-objcopy
AVRDUDE := avrdude

NATIVE_GCC := gcc

SOURCES := loop.cpp Voice.cpp Envelope.cpp midi.cpp wave_function.c circbuf_tiny.c pool_tiny.c
HEADERS := circbuf_tiny.h pool_tiny.h debug.h Envelope.h hardware.h instruments.h midi.h Voice.h wave_function.h

CFLAGS += -flto -DF_CPU=16000000UL -mmcu=atmega328p
#CFLAGS += -Os #27.3us
CFLAGS += -O3 #26.4us
#CFLAGS += -Ofast #26.4
CFLAGS += -Wall -Wextra
CFLAGS += -g 

CXXFLAGS := $(CFLAGS) -std=c++14
CFLAGS += -std=c11


LDFLAGS += -Wl,--gc-sections -fuse-linker-plugin
#LDFLAGS += -flto-report
LDLIBS += -lc


OBJS := $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(SOURCES)) )
OBJS += avr.o
OBJDIR := ./objs-make

all: softsynth.elf size softsynth.eep softsynth.hex header_deps

#Really simple header file dependencies - rebuild everything
header_deps: $(HEADERS)
	echo "rebuild due to header change"
	touch header_deps

softsynth.elf: $(addprefix $(OBJDIR)/, $(OBJS))
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS)

$(OBJDIR)/%.o: %.c header_deps sin_table.h
	$(CC) -c -o $@ $(CFLAGS) $<

$(OBJDIR)/%.o: %.cpp header_deps voice_notes.h
	$(CXX) -c -o $@ $(CXXFLAGS) $<

$(OBJDIR)/%.o: %.S
	$(CXX) -c -o $@ $(CXXFLAGS) $<

size: softsynth.elf
	$(SIZE) --mcu=atmega328p -C --format=avr $<

softsynth.eep: softsynth.elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom='alloc,load' --no-change-warnings --change-section-lma .eeprom=0 -O ihex $<  $@

softsynth.hex: softsynth.elf
	$(OBJCOPY) -O ihex -R .eeprom  $< $@

voice_notes.h: ./utils/notes.c header_deps
	$(NATIVE_GCC) -Wall -o ./utils/bin/notes $< -lm
	./utils/bin/notes 440 -p > voice_notes.h

sin_table.h: ./utils/sin_generate.c header_deps
	$(NATIVE_GCC) -Wall -o ./utils/bin/sin $< -lm
	./utils/bin/sin > sin_table.h 

clean:
	rm -f softsynth.elf softsynth.eep softsynth.hex header_deps voice_notes.h sin_table.h
	rm -f ./utils/bin/sin ./utils/bin/notes
	cd $(OBJDIR) && rm -f *.o

upload: softsynth.hex size
	$(AVRDUDE) -q -V -p atmega328p -C $(AVR_TOOLS_PATH)/etc/avrdude.conf -D -c arduino -b 115200 -P /dev/ttyACM0 -U flash:w:softsynth.hex:i
