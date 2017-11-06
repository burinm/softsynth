/*  loop.cpp - Invented Here License v1.0 (2017) - burin

    Driver for 8-bit soft synth on Arduino Atmega328p
    setup(), loop() 

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

using namespace SoftSynth;

/* Voices */
extern const uint8_t max_voices;
const uint8_t max_voices=3;
Voice voices[max_voices];

extern circbuf_tiny_t *midi_buf;

/*
We are tuning to A at    440.000 Hz
Machine is running at 16000000.000 Hz
Samples are at         44100.000 Hz
Synth timer (samples) will need 363 machine ticks
-------------------------------------------------
*/

/* Notes:
*/

envelope_t flute_instrument = {

    .attack_ticks =     16,
    .attack_count =     127,
    .decay_ticks =      16,
    .decay_count =      20,
    .sustain_ticks =    4096,
    .sustain_hold =     1,
    .release_ticks =    16,
    .release_count =    107,
};

envelope_t flute_instrument2 = {

    .attack_ticks =     16,
    .attack_count =     127,
    .decay_ticks =      16,
    .decay_count =      20,
    .sustain_ticks =    4096,
    .sustain_hold =     1,
    .release_ticks =    16,
    .release_count =    107,
};


static int8_t dither_random_table[128] = {
    1,1,-2,0,0,0,-1,1,1,1,-2,-1,1,0,0,0,
    0,1,-1,1,2,0,0,1,0,-1,-2,0,2,0,-1,0,
    -2,-1,2,1,-2,-1,0,-1,0,0,0,1,2,-1,-1,-2,
    0,0,-1,0,0,1,0,2,-1,-2,1,0,2,2,-2,2,
    1,-1,1,2,-1,1,-1,1,-1,0,-1,2,1,1,-1,0,
    0,1,0,0,1,-1,0,1,0,-2,-1,1,2,0,0,-1,
    1,-1,-2,0,0,-2,-2,0,-2,-1,0,-2,0,0,1,-1,
    2,2,-1,-2,2,-1,0,-2,1,-1,0,0,0,-1,-1,-2,
};



void setup() {
wdt_disable();
    // set the digital pin as output:  

    //8-bit digital sound out
    DDRD = 0B11111110; //pins 6-7 output, sound lsb, pin2-5 debug, pins 0-1 reserved for UART
    //DDRD = 0B11111111;
    PORTD = 0x0;

    //8-bit sound out msb
    DDRB = 0B00111111; //pins 0-5 output, sound msb
    cli();
    
    //Defaults for timer registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    OCR1A = (CPU_SPEED/SAMPLE_RATE) +1 ; //round up
    TCCR1B |= (1 << WGM12 );   // CTC mode, top is OCR1A
    TCCR1B |= (1 << CS10);    // x1 prescaler (no prescaling)
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt


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

    /* Initalize Voices */
    voices[0].init(t_pulse, flute_instrument);
    //voices[0].init(t_triangle, flute_instrument);
    //voices[0].init(t_noise, flute_instrument);
    voices[1].init(t_sin, flute_instrument2);
    //voices[2].init(t_sawtooth, flute_instrument2);

    //voices[2].init(t_sin, flute_instrument2);
    voices[2].init(t_triangle, flute_instrument2);
    //voices[2].init(t_noise, flute_instrument2);

    sei();


}

ISR(BADISR_vect)
{
//PORTD |=0xf0;
}

#if 0
//vector_16.....grrrr timer zero overflow default
ISR(TIMER0_OVF_vect )
{

}
#endif

void loop()
{

;    set_sleep_mode(SLEEP_MODE_IDLE);
;    sleep_enable();
;    sleep_mode();
}

//Update Synth routine
ISR(TIMER1_COMPA_vect) { //(45us allowed)                                                  //17.02 running, 14.60us
//static uint16_t synth_clock=0;

//For speed, don't allocate these every time
static uint16_t mixer;
static uint8_t sample;
static uint8_t i=0;
static uint16_t random_number2 = 0;
static uint16_t random_number_count = 0;

// Play sample first thing for timing reasons. This means notes
//  will possibly be behind 1/22.05Hz

//Because all 8 pins on any port can't be used with the UART at the same time,
// two ports must be used for the digital output. Put the MSBs on the port
// with the most sucessive pins on the same port PORTB(6), because we can't
// synchronize writing to two ports at once. This will be a less glitchy sound
// since just the lowest two bits may not synchonize.


    //Highest two bits set to audio lsb, Lower 2 bits reserved for UART
    // This also leaves debug bits 2-5 alone
    //PORTD |= sample<<6;
    mixer <<= 2;
    PORTD |= (mixer & 0xFC);
    //PORTD |= (0xfff<<2) & 0xFC;

    //Put most significant bits here, so they all change at once
    PORTB = (mixer & 0x3f00) >>8;
    //PORTB = (0xfff>>6) & (0x3f);

    process_midi_messages();                 // 1.2us

                                             //11.6us running
ERROR_SET(ERROR_MARK);
#if 1
    mixer=0;
    for (i=0;i<max_voices;i++) {
        voices[i].step();                       //7.6us running
        mixer += (voices[i].sample());          //5.0us running
    }
#endif
ERROR_SET(ERROR_MARK);

    #if 0
    //Dithering
    if (random_number_count == 8000) {                          //2.96us
        random_number_count = 0;
        random_number2++;
        if (random_number2 == 128) { random_number2 = 0; }
    }
    random_number_count++;

    mixer += dither_random_table[random_number2];
    #endif

    mixer <<= 2; //Only using 10bits right now
    //mixer &= (0xfff); //12 bit audio mask

    //sample = (uint8_t)(mixer & 0xff);
}

ISR(USART_RX_vect) {
volatile uint8_t b;

ERROR_SET(ERROR_RECEIVE);

    if (UCSR0A & (1<<RXC0)) {
        b = (uint8_t)UDR0;
        /* Critial section, but on Atmel328p, interrupt handler is safe */
        if (circbuf_tiny_write(midi_buf,b) == 0) {
            ERROR_SET(ERROR_OVERFLOW);
        }
        /* end critical */
    }

ERROR_SET(ERROR_RECEIVE);
}
