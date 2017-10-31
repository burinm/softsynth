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
#include "circbuf_tiny.h"
#include "midi.h"
#include "wave_function.h"
}

#include "Voice.h"
#include "Envelope.h"

using namespace SoftSynth;

void error_set(uint8_t e);
enum error_type{ERROR_NONE=0,              // xxxx
                ERROR_MARK =  0x1,         // Oxxx
                ERROR_FATAL = 0x2,         // xOxx
                ERROR_CRITICAL = 0x4,      // xxOx 
                ERROR_OVERFLOW = 0x8,      // xxxO
                ERROR_RECEIVE = 0x6,       // xOOx 
                ERROR_ALL=0xf};            // OOOO

/* Voices */
#define MAX_VOICES  1
Voice voices[MAX_VOICES];

/* midi buffer */
circbuf_tiny_t *midi_buf;
uint8_t midi_running_status = MIDI_STATUS_NOTE_OFF;
uint8_t midi_current_channel = MIDI_DEFAULT_CHANNEL;

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

void setup() {
wdt_disable();
    // set the digital pin as output:  

    //8-bit digital sound out
    DDRD = 0B11111100; //pins 6-7 output, sound lsb, pin2-5 debug, pins 0-1 reserved for UART
    //DDRD = 0B11111111;
    PORTD = 0x0;

    //8-bit sound out msb
    DDRB = 0B00111111; //pins 0-5 output, sound msb
    cli();
    
    //Defaults for timer registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    //Timer will fire every 44.1kHz, CD quality, lol - Nyquist sampling theory on limit of human hearing ~20kHz

    //16MHz clocked Arduino - .625ns clock * 363 clocks = 44077Hz
    
    //OCR1A = 726;            
    //OCR1A = 900;            
    OCR1A = 363;            
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
             (1<<TXEN0) |  /*  Enable TX */
             (1<<RXCIE0);  /* Enable RX complete interrupt */

    /* Set frame format: 8N1 */
    UCSR0C = (3<<UCSZ00);


    /* initialize midi buffer */
    if ( (midi_buf = (circbuf_tiny_t*)malloc(sizeof(circbuf_tiny_t))) == 0) {
        error_set(ERROR_FATAL);
    }

    if (circbuf_tiny_init(midi_buf) == 0) {
        error_set(ERROR_FATAL);
    }

    /* Initalize Voices */
    voices[0].init(t_pulse, flute_instrument);
    //voices[1].init(t_sin, flute_instrument2);
    //voices[2].init(t_pulse, flute_instrument2);
    //voices[3].init(t_noise, flute_instrument2);

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
ISR(TIMER1_COMPA_vect) {
static uint16_t synth_clock=0;

//For speed, don't allocate these every time
static uint16_t mixer;
static uint8_t sample;

//Debug toggle - This adds noise to the port...
//PORTD |= (1<<2);
//error_set(ERROR_MARK);

//Since our phase is 1024, and our multiplier is 64
// This will overflow correctly. 2^10 + 2^6
synth_clock++;


#if 1
    /* Critial section, but on Atmel328p, interrupt handler is safe */
    static uint8_t byte_out;
    if (CIRCBUF_TINY_SIZE(midi_buf) > 0)
    {
        //Get byte from buffer
        circbuf_tiny_read(midi_buf, &byte_out);

    #if 1 //Midi processing
        static uint8_t midi_byte_number = 0; 

        if (byte_out & MIDI_STATUS_MASK) {
            midi_byte_number=0;

            midi_running_status=(byte_out & MIDI_STATUS_TYPE_MASK) >> MIDI_STATUS_TYPE_OFFSET;
            midi_current_channel = byte_out & MIDI_STATUS_CHANNEL_MASK;

            if (midi_running_status == MIDI_STATUS_RESET) {
                //synth.reset();
            }
        } else {


            switch(midi_running_status) {
                case    MIDI_STATUS_NOTE_ON:
                    if (midi_byte_number == 0) {
                        if(midi_current_channel < MAX_VOICES) {
                            voices[midi_current_channel].startNote(byte_out);
                        }
                        midi_byte_number++;
                        break;
                    }

                    if (midi_byte_number == 1) {
                        if (byte_out == 0 ) { //zero velocity = NOTE_OFF
                            if(midi_current_channel < MAX_VOICES) {
                                voices[midi_current_channel].stopNote();
                            }
                        }
                        //voice0.velocity = byte_out;
                        midi_byte_number=0;
                        break;
                    }
                    break;
                case    MIDI_STATUS_NOTE_OFF:
                        if(midi_current_channel < MAX_VOICES) {
                            voices[midi_current_channel].stopNote();
                        }
                        midi_byte_number=0;
                    break;
                default:
                    midi_byte_number = 0;
                    break;
            }
        #endif //end midi processing

        }
    }
    /* end critical */
#endif


error_set(ERROR_MARK);


#if 0
current_voice++;
if (current_voice == 3) { current_voice = 0; }

switch(current_voice) {
    case 0:
       voice0.step();
        sample = voice0.sample(synth_clock);
    break;

    case 1:
       voice1.step();
        sample = voice1.sample(synth_clock);
    break;

    case 2:
       voice2.step();
        sample = voice2.sample(synth_clock);
    break;
}
#endif
error_set(ERROR_MARK);

mixer=0;
for (uint8_t i=0;i<MAX_VOICES;i++) {
    voices[i].step();
    mixer += voices[i].sample(synth_clock);
}
//mixer >>= 1;
sample = (uint8_t)mixer;

//mixer >>= (MAX_VOICES>>1);
//mixer >>= 1;
//sample2 = voices[1].sample(synth_clock);
//sample3 = voices[2].sample(synth_clock);
//sample4 = voices[3].sample(synth_clock);



//mixer=(sample -127) + (sample2 -127) + (sample3 -127);
//mixer=(sample -127) + (sample2 -127);
//mixer += 127;
//mixer= sample + sample2 + sample3 + sample4;
//mixer /= 3;



//Because all 8 pins on any port can't be used with the UART at the same time,
// two ports must be used for the digital output. Put the MSBs on the port 
// with the most sucessive pins on the same port PORTB(6), because we can't 
// synchronize writing to two ports at once. This will be a less glitchy sound
// since just the lowest two bits may not synchonize. 

//Highest two bits set to audio lsb, Lower 2 bits reserved for UART
// This also leaves debug bits 2-5 alone
PORTD |= sample<<6;
//Put most significant bits here, so they all change at once
PORTB = sample>>2;

// Currently running 9.4us per interupt
//  Allowed = 22.6 us <-> 44.1kHz

#if 0
    //send UART test
    if ( UCSR0A & (1<<UDRE0) ) {
        UDR0 = 'U';
    }
#endif


//error_set(ERROR_MARK);
}

void error_set(uint8_t e) {
    switch(e) {
        case ERROR_MARK:
        case ERROR_RECEIVE:
            PORTD ^= (e & 0xf) << 2;
            break;
        case ERROR_FATAL:
             PORTD |= (e & 0xf) << 2;
             for(;;);
             break;
        default:
            PORTD |= (e & 0xf) << 2;

    }
}



ISR(USART_RX_vect) {
volatile uint8_t b;

error_set(ERROR_RECEIVE);

    if (UCSR0A & (1<<RXC0)) {
    
        b = (uint8_t)UDR0;
        /* Critial section, but on Atmel328p, interrupt handler is safe */
        if (circbuf_tiny_write(midi_buf,b) == 0) {
            error_set(ERROR_OVERFLOW);
        }
        /* end critical */
    }
    

error_set(ERROR_RECEIVE);
}


