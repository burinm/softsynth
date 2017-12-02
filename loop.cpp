/*  loop.cpp - Invented Here License v1.0 (2017) - burin

    Driver for 8-bit soft synth on Arduino Atmega328p
    setup(), loop() 

*/

/* TODO: The big list

    0) - profile midi buffer
    1) - fix noise channel 
    2) - fix envelope generator
        o click gone
        o need to check steps
        o Bug when Decay control is set, Sustain button doesn't work
    2) - polyphony per channel?
    3) - update linux emulator for new timer scheme
    4) - cirbuf static, fix cirbuf impl
    5) - voice pools
    6) - noise channel is slow at high frequencies
    7) - Midi command (start note, <0x79, note_off velocity) broken?

*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/wdt.h> //wdt_disable, wdt_reset
#include <avr/sleep.h>

extern "C" {
#include "hardware.h"
#include "circbuf_tiny.h"
#include "wave_function.h"
}

#include "debug.h"

#include "Voice.h"
#include "Envelope.h"
#include "midi.h"
#include "instruments.h"

#if 0
extern "C" {
    void DELAY_5();
    void jitter_undo();
};
#endif

using namespace SoftSynth;

/* Voices */
extern const uint8_t max_voices;
const uint8_t max_voices=4;
Voice voices[max_voices];

extern circbuf_tiny_t midi_buf;

void setup() {
//wdt_disable(); //clang won't compile this due to overflow on "I" parameter

// From Atmega328p datasheet, disable watchdog
cli();
wdt_reset();
/* Clear WDRF in MCUSR */
MCUSR &= ~(1<<WDRF);
/* Write logical one to WDCE and WDE */
/* Keep old prescaler setting to prevent unintentional time-out */
WDTCSR |= (1<<WDCE) | (1<<WDE);
/* Turn off WDT */
WDTCSR = 0x00;
sei();

    // set the digital pin as output:  

    //12-bit digital sound out
    DDRD = 0B11111110; //pins 2-7 (sound lsb), pin 1 reserved for debug, pin 0 reserved for UART RX
    PORTD = 0x0;

    //8-bit sound out msb
    DDRB = 0B00111111; //pins 0-5 (sound msb)
    PORTB = 0x0;

    /* Initalize Voices */
    //voices[0].init(t_pulse, flute_instrument1);
    voices[0].init(t_sin, fatty_base_instrument1);
    //voices[0].init(t_triangle, flute_instrument);
    //voices[0].init(t_noise, flute_instrument);

    voices[1].init(t_pulse, flute_instrument2);
    //voices[1].init(t_sawtooth, flute_instrument2);

    //voices[2].init(t_triangle, flute_instrument2);
    //voices[2].init(t_sawtooth, flute_instrument2);
    //voices[2].init(t_sawtooth, flute_instrument2);
    voices[2].init(t_noise, drum_instrument1);


    //voices[3].init(t_noise, flute_instrument2);
    //voices[3].init(t_pulse, fatty_base_instrument1);
    voices[3].init(t_triangle, fatty_base_instrument1);


    //Interrupts off to setup timers
    cli();

// Timer 0, for Sample Loop
    //Defaults for timer0 registers
    TCCR0A = 0;
    TCCR0B = 0;

#if 1
    //OCR0A = ((CPU_SPEED/8)/SAMPLE_RATE) +1;
    OCR0A = ((CPU_SPEED/8)/SAMPLE_RATE);
    //OCR0A = ((CPU_SPEED/64)/SAMPLE_RATE) +1;
    //OCR0A = ((CPU_SPEED/64)/SAMPLE_RATE);
    OCR0B =0;
    TCCR0B |= (1 << CS01); // divide 8 prescale
    //TCCR0B |= ( (1 << CS00) | (1 << CS01)); // divide 64 prescale
    TCCR0A |= (1 << WGM01); //CTC mode, top is OCR0A
    TIMSK0 |= (1 << OCIE0A); //enable timer compare interrupt
#endif


// Timer 1, 16bit timer, overflow for Tone Clock
    //Defaults for timer1 registers
    TCCR1A = 0;
    TCCR1B = 0;

#ifndef NON_CORRECTING_TIMING
    //Timer will run 62500Mz:
    OCR1A = 0; //Full 16 bits
    TCCR1B |= (1 << CS12);    // divide 256 prescale 

    //OCR1A = (CPU_SPEED/SAMPLE_RATE); //Full 16 bits
    //TCCR1B |= (1<<CS10) | (1 << CS12);    // divide 1024 prescale 
    //TCCR1B |= (1 << WGM12); //CTC mode, top is OCR1A
    //TCCR1B |= (1 << CS10);    // no prescaler, enabled
    //TIMSK1 |= (1 << OCIE1A); //enable timer compare interrupt
#endif


//override some gcc stuff...

//This timer seems to be on by default!!
TIMSK0 &= ~_BV(TOIE0); // disable timer0 overflow interrupt
//unsigned int *pvector_16=(unsigned int*)0x40;
 //   *pvector_11=0x0000;

    #define BAUD_MIDI    31250
    #define BAUD_LINUX   38400 
    //Setup serial port - USART UART
    #define FOSC    16000000
    #define BAUD    BAUD_LINUX
    //#define BAUD    BAUD_MIDI
    #define UBRR    ((FOSC/16/BAUD) - 1)

    /*Set baud rate */
    UBRR0H = (uint8_t)(UBRR>>8);
    UBRR0L = (uint8_t)UBRR;
    UCSR0B = (1<<RXEN0)  /* Enable receive */
             // |
             //(1<<TXEN0) |  /*  Enable TX - Use this pin for debug instead*/ 
             //(1<<RXCIE0)  /* Enable RX complete interrupt */
    ;

    /* Set frame format: 8N1 */
    UCSR0C = (3<<UCSZ00);

    /* Init midi and midi buffer */
    midi_init();

    sei();
}

ISR(BADISR_vect)
{
//PORTD |=0xf0;
}


int main()
{
setup();

    for(;;) {
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_enable();
        sleep_mode();
    }
}

//For speed, don't allocate these every time
static uint16_t mixer;
static uint8_t i=0;
static uint8_t portd_tmp;
static uint16_t fast_timer=0;


//Interrupts are off on the AVR Atmel328p achitechture
ISR(TIMER0_COMPA_vect) { //Update output sample routine

//Couldn't get this to work without spending precious cycles
//jitter_undo();

ERROR_SET(ERROR_MARK); //Diagnostics cause pops, clicks

#ifdef NON_CORRECTING_TIMING 
    /*
        Sounds better than timer read
         but will wobble if timing loop goes long
    */
    fast_timer+= SAMPLE_DIVIDER;
#else
    /*
        Should sychronize if timing goes long
         But high notes are unstable 
    */
    fast_timer= TCNT1;
#endif

// Play sample first thing for timing reasons.
//

/*
    12 bit digital audio!

    PORTD - bit  0  , UART RX
            bit  1  , UART TX - can ignore UART, and use as debug out 
            bits 2-7, LSBits of output 

    PORTB - bits 0-5, MSBits of output
            bits 6-7, unavailable

     Change the MSBs last because we can't synchronize writing to
      two ports at once. This will be a less glitchy sound
      in theory..
*/

    // LSBits - This leaves TX and RX (now debug) bits 0-1 alone
    portd_tmp = (PORTD & 0x3);
    PORTD = portd_tmp | ((uint8_t)(mixer & 0x3F) << 2);

    // MSBits - Mask out unavailable bits 6,7
    PORTB = (mixer & 0xFC0) >>6;

    process_midi_messages();            //2us

    mixer=0;
#if 1
    for (i=0;i<max_voices;i++) {
        voices[i].step(fast_timer);
        mixer += (voices[i].sample());
    }
#endif

    mixer <<= 2; //Only using 10bits right now
    //mixer &= (0xfff); //12 bit audio mask

    /*
        38400 fastest MIDI bit rate,  10bits/byte = 3840Hz 
         so, just checking each sample is plenty fast
         enough to ovoid UART buffer overflow.
        No need to complicate timing with another interrupt
    */
    static uint8_t b;
    if (UCSR0A & (1<<RXC0)) { 
        b = (uint8_t)UDR0;
        if (circbuf_tiny_write(&midi_buf,b) == 0) {
            ERROR_SET(ERROR_OVERFLOW);
        }
    }
ERROR_SET(ERROR_UNMARK);
}

#if 0
ISR(USART_RX_vect) {
volatile uint8_t b;

    if (UCSR0A & (1<<RXC0)) {
        b = (uint8_t)UDR0;
        /* Critial section, but on Atmel328p, interrupt handler is safe */
        if (circbuf_tiny_write(&midi_buf,b) == 0) {
            ERROR_SET(ERROR_OVERFLOW);
        }
        /* end critical */
    }
}
#endif
