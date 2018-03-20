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
    8) - FastVoice drum - 1st note too short?, if held down subsequent notes long 

*/

#include <avr/io.h>
#include <avr/interrupt.h>

//#include <avr/wdt.h> //wdt_disable, wdt_reset
#include <avr/sleep.h>

extern "C" {
#include "hardware.h"
#include "circbuf_tiny.h"
#include "wave_function.h"
}

#include "debug.h"

#ifdef FASTVOICE
    #include "VoiceFast.cpp"
#else
    #include "Voice.h"
    #include "instruments.h"
#endif

#include "Envelope.h"
#include "midi.h"

using namespace SoftSynth;


/* Voices */
#ifdef FASTVOICE
    VoiceFast voices[MAX_VOICES];
#else
    Voice voices[MAX_VOICES];
#endif

extern circbuf_tiny_t midi_buf;

void setup() {
//wdt_disable(); //clang won't compile this due to overflow on "I" parameter

cli();
    // From Atmega328p datasheet, disable watchdog
    __asm__ __volatile__ ("wdr");
    //wdt_reset();
    /* Clear WDRF in MCUSR */
    MCUSR &= ~(1<<WDRF);
    /* Write logical one to WDCE and WDE */
    /* Keep old prescaler setting to prevent unintentional time-out */
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    /* Turn off WDT */
    WDTCSR = 0x00;
sei();


    //12-bit digital sound out

    /* PORTD
        pins 2-7 (sound lsb),       output  
        pin 1    for debug,         output
        pin 0 reserved for UART RX
    */
    DDRD = 0B11111110; 
    PORTD = 0x0;

    /* PORTB
        pins 6-7 unused
        pins 0-5 (sound lsb),       output  
    */
    DDRB = 0B00111111; //pins 0-5 (sound msb)
    PORTB = 0x0;

#ifdef FASTVOICE
    voices[0].init(t_sin);
    voices[1].init(t_pulse);
    voices[2].init(t_noise); voices[2].setDrum();
    voices[3].init(t_triangle);
    voices[4].init(t_sawtooth);
    //voices[0].init(t_triangle);
#else
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
#endif


//Interrupts off to setup timers
cli();

#ifdef FASTVOICE
// Timer 0, for Drum decay
    //Defaults for timer0 register
    TCCR0A = 0;
    TCCR0B = 0;

    /*
        Never let the timer in CTC mode reach TOP,
            keep resetting at match (COMPA) DRUM_DECAY - 1.

        Then trigger TOP by setting TCNT0 above OCR0A
            (COMPA) elsewhere in the code.

        This is a way of implementing a one-shot
    */
    OCR0A = DRUM_DECAY - 1; //Set below so we never reach TOP

    // Set prescaler, timer on
#if DRUM_TIMER_DIV == 1
    TCCR0B |= (1 << CS00); // timer on, no prescale 
#elif DRUM_TIMER_DIV == 8
    TCCR0B |= (1 << CS01); // divide 8 prescale
#elif DRUM_TIMER_DIV == 64
    TCCR0B |= ( (1 << CS00) | (1 << CS01)); // divide 64 prescale
#elif DRUM_TIMER_DIV == 256
    TCCR0B |= (1 << CS02); // 256 untested
#elif DRUM_TIMER_DIV == 1024
    TCCR0B |= ((1 << CS02) | (1 << CS00)); // divide 1024
#else
    #error "DRUM_TIMER_DIV value not supported for this platform/implementation"
#endif

    TCCR0A |= (1 << WGM01); //CTC mode, top is OCR0A
    //TIMSK0 |= (1 << OCIE0A) |  (1 << TOIE0); //enable timer compare/top interrupts
    TIMSK0 |=   (1 << TOIE0); //enable timer compare/top interrupts
#endif


// Timer 1, 16bit timer, read to calculate phase
    //Defaults for timer1 registers
    TCCR1A = 0;
    TCCR1B = 0;

    //Timer will run 62500Mz:
    OCR1A = 0; //Full 16 bits
    TCCR1B |= (1 << CS12);                    // divide 256 prescale

    #define BAUD_MIDI    31250 //MIDI specification
    #define BAUD_LINUX   38400 //Standard serial port speed on an x86 machine

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

//For speed, don't allocate these every time
static uint16_t mixer;
static uint8_t i=0;
static uint8_t portd_tmp;
static uint16_t fast_timer=0;


//Interrupts are off on the AVR Atmel328p achitechture
//Update output sample routine
int main()
{
setup();

    //Run a loop as fast as we can, then read the timer to see where we are
    // i.e. "infinite sample rate"
    for(;;) {
        //ERROR_SET(ERROR_MARK); //Diagnostics cause pops, clicks

        fast_timer = TCNT1 ;

        // Play sample first thing for timing reasons.
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

            process_midi_messages();

            mixer=0;
            for (i=0;i<MAX_VOICES;++i) {
                mixer += (voices[i].sample(fast_timer));
            }

            #ifdef POLYPHONY
                 mixer <<=1; //11 bit audio mask (pout of 12)
            #else
                #ifdef FASTVOICE
                    mixer <<= 1;
                #else //no special settings
                    mixer <<= 2; //Only using 10bits right now
                #endif
            #endif

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
        //ERROR_SET(ERROR_UNMARK);
    }
}

#ifdef FASTVOICE
    ISR(TIMER0_OVF_vect) {
        /*
            Naive implementation for speed,
             really should loop through all voices
             for each, if V->is_drum then V->drum off
        */
        voices[2].drum_on = 0;
    }
#endif

