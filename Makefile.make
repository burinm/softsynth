
AVR_TOOLS_PATH := /opt/avr-tools
CC := avr-gcc
CXX := avr-g++
SIZE := avr-size
OBJCOPY := avr-objcopy
AVRDUDE := avrdude

SOURCES := loop.cpp Voice.cpp Envelope.cpp midi.cpp wave_function.c circbuf_tiny.c
HEADERS := circbuf_tiny.h debug.h Envelope.h hardware.h instruments.h midi.h Voice.h wave_function.h

CFLAGS += -Os -flto -DF_CPU=16000000UL -mmcu=atmega328p
CFLAGS += -Wall -Wextra

CXXFLAGS := $(CFLAGS) -std=c++14
CFLAGS += -std=c11


LDFLAGS += -Wl,--gc-sections -fuse-linker-plugin
#LDFLAGS += -flto-report
LDLIBS += -lc


OBJS := $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(SOURCES)) )
OBJDIR := ./objs-make

all: softsynth.elf size softsynth.eep softsynth.hex header_deps

#Really simple header file dependencies - rebuild everything
header_deps: $(HEADERS)
	echo "rebuild due to header change"
	touch header_deps

softsynth.elf: $(addprefix $(OBJDIR)/, $(OBJS))
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS)

$(OBJDIR)/%.o: %.c header_deps
	$(CC) -c -o $@ $(CFLAGS) $<

$(OBJDIR)/%.o: %.cpp header_deps
	$(CXX) -c -o $@ $(CXXFLAGS) $<

size: softsynth.elf
	$(SIZE) --mcu=atmega328p -C --format=avr $<

softsynth.eep: softsynth.elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom='alloc,load' --no-change-warnings --change-section-lma .eeprom=0 -O ihex $<  $@

softsynth.hex: softsynth.elf
	$(OBJCOPY) -O ihex -R .eeprom  $< $@

clean:
	rm -f softsynth.elf softsynth.eep softsynth.hex
	cd $(OBJDIR) && rm -f *.o

upload: softsynth.hex
	$(AVRDUDE) -q -V -p atmega328p -C $(AVR_TOOLS_PATH)/etc/avrdude.conf -D -c arduino -b 115200 -P /dev/ttyACM0 -U flash:w:softsynth.hex:i
