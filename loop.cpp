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

*/

#include <Arduino.h>

#include <math.h>

#include <avr/wdt.h> //wdt_disable
#include <avr/io.h>
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

using namespace SoftSynth;

/* Voices */
extern const uint8_t max_voices;
const uint8_t max_voices=4;
Voice voices[max_voices];

extern circbuf_tiny_t midi_buf;

void setup() {
wdt_disable();
    // set the digital pin as output:  

    //12-bit digital sound out
    DDRD = 0B11111110; //pins 2-7 (sound lsb), pin 1 reserved for debug, pin 0 reserved for UART RX
    PORTD = 0x0;

    //8-bit sound out msb
    DDRB = 0B00111111; //pins 0-5 (sound msb)

    /* Initalize Voices */
    //voices[0].init(t_pulse, flute_instrument);
    voices[0].init(t_sin, flute_instrument1);
    //voices[0].init(t_triangle, flute_instrument);
    //voices[0].init(t_noise, flute_instrument);
    voices[1].init(t_pulse, flute_instrument2);
    //voices[2].init(t_sawtooth, flute_instrument2);

    //voices[2].init(t_triangle, flute_instrument2);
    //voices[2].init(t_sawtooth, flute_instrument2);
    //voices[2].init(t_sawtooth, flute_instrument2);
    voices[2].init(t_noise, drum_instrument1);


    //voices[3].init(t_noise, flute_instrument2);
    //voices[3].init(t_pulse, fatty_base_instrument1);
    voices[3].init(t_triangle, fatty_base_instrument1);


    //Interrupts of to setup timers
    cli();

// Timer 0, for Sample Loop
    //Defaults for timer0 registers
    TCCR0A = 0;
    TCCR0B = 0;

    //OCR0A = ((CPU_SPEED/8)/SAMPLE_RATE) +1;
    OCR0A = ((CPU_SPEED/64)/SAMPLE_RATE) +1;
    OCR0B =0;
    //TCCR0B |= (1 << CS01); // divide 8 prescale
    TCCR0B |= ( (1 << CS00) | (1 << CS01)); // divide 64 prescale
    TCCR0A |= (1 << WGM01); //CTC mode, top is OCR0A
    TIMSK0 |= (1 << OCIE0A); //enable timer compare interrupt

    
// Timer 1, 16bit timer, overflow for Tone Clock
    //Defaults for timer1 registers
    TCCR1A = 0;
    TCCR1B = 0;

    OCR1A = 0; //Full 16 bits
    TCCR1B |= (1 << CS12);    // divide 256 prescaler


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
    #define UBRR    ((FOSC/16/BAUD) - 1)

    /*Set baud rate */
    UBRR0H = (uint8_t)(UBRR>>8);
    UBRR0L = (uint8_t)UBRR;
    UCSR0B = (1<<RXEN0) |  /* Enable receive */
             //(1<<TXEN0) |  /*  Enable TX - Use this pin for debug instead*/ 
             (1<<RXCIE0);  /* Enable RX complete interrupt */

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


void loop()
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
}

//For speed, don't allocate these every time
static uint16_t mixer;
static uint8_t sample;
static uint8_t i=0;
static uint16_t fast_timer=0;

static uint8_t portd_tmp;


ISR(TIMER0_COMPA_vect) { //Update output sample routine

//interrupts should be off inside here 
fast_timer= TCNT1;


// Play sample first thing for timing reasons. This means notes
//  will possibly be behind 1/22.05Hz

//Because all 8 pins on any port can't be used with the UART at the same time,
// two ports must be used for the digital output. Put the MSBs on the port
// with the most sucessive pins on the same port PORTB(6), because we can't
// synchronize writing to two ports at once. This will be a less glitchy sound
// since just the lowest two bits may not synchonize.


    //Highest 6 bits set to audio lsb, Lower 2 bits reserved for debug, UART RX
    // This also leaves debug bits 2-5 alone
    portd_tmp = (PORTD & 0x3);
    PORTD = portd_tmp | ((uint8_t)(mixer & 0x3F) << 2);

    //Put most significant bits here, so they all change at once
    PORTB = (mixer & 0xFC0) >>6;

    process_midi_messages();

ERROR_SET(ERROR_MARK);
    mixer=0;
#if 1
    for (i=0;i<max_voices;i++) {
        voices[i].step(fast_timer);
        mixer += (voices[i].sample());
    }
#endif
    //voices[0].step(fast_timer);
    //mixer = voices[0].sample();

ERROR_SET(ERROR_MARK);

    mixer <<= 2; //Only using 10bits right now
    //mixer &= (0xfff); //12 bit audio mask
}

ISR(USART_RX_vect) {
volatile uint8_t b;

ERROR_SET(ERROR_RECEIVE);

    if (UCSR0A & (1<<RXC0)) {
        b = (uint8_t)UDR0;
        /* Critial section, but on Atmel328p, interrupt handler is safe */
        if (circbuf_tiny_write(&midi_buf,b) == 0) {
            ERROR_SET(ERROR_OVERFLOW);
        }
        /* end critical */
    }

ERROR_SET(ERROR_RECEIVE);
}
