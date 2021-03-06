#include <cstdio>
#include <cstdint>
#include <cstdlib>

//#include <csignal>
#include <ctime>

#include <cmath> //round
#include <sched.h> //yield

#include "test_midi.h"
#ifdef FASTVOICE
    #include "../VoiceFast.cpp"
#else
    #include "../Voice.h"
#endif
#include "../Envelope.h"
#include "../midi.h"
#include "../instruments.h"

extern "C" {
    #include "../wave_function.h"
    #include "../hardware.h"
}

using namespace SoftSynth;


void ctrl_c(int);
void timer0_sample_loop(int sig, siginfo_t *si, void *uc);

void setup_timer();


void setup_timer() {
#define PERIOD_CLOCK_SAMPLE ( 1000000000 / SAMPLE_RATE )
//#define PERIOD_CLOCK_SAMPLE 64000 

/*
    The Atmel328p runs 16MHz, with a divide by 256 scaled clock
    for the sample loop - 16MHz/256 = 62500

    In the linux port, the most stable way to track this seems
    to be assuming the sample loop signal popping is accurate.
    Measuring the time with clock_gettime(MONOTONIC) causes jitters.

    In order to use the same counting mechanism where 2^16
    represents one cycle, match the Atmel328p clock rate
    with an exact multiple, the closest one is

    62500/4 = 15625 

    This way fast_timer +=4 can be used in the loop
*/


/* Copied almost verbatim from timer_create(2) man page
    Linux Programmer's Manual
*/

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

static timer_t sample_loop_timerid;
static struct sigevent loop_sev;
static struct itimerspec loop_its;
static long long freq_nanosecs;
static sigset_t mask;
static struct sigaction loop_sa;

           /* Establish handler for sample loop timer signal */
           fprintf(stderr, "Establishing handler for signal %d\n", SAMPLE_CLOCK_SIGNAL);
           loop_sa.sa_flags = SA_SIGINFO;
           loop_sa.sa_sigaction = timer0_sample_loop;
           //sigemptyset(&loop_sa.sa_mask);
            //Block I/O (serial) midi signals, tone clock
           sigaddset(&loop_sa.sa_mask, MIDI_INPUT_SIGNAL);
           if (sigaction(SAMPLE_CLOCK_SIGNAL, &loop_sa, NULL) == -1)
               errExit("sigaction, SAMPLE_CLOCK_SIGNAL");

           /* Block timer signals temporarily */
           fprintf(stderr, "Blocking signal %d\n", SAMPLE_CLOCK_SIGNAL);
           sigemptyset(&mask);
           sigaddset(&mask, SAMPLE_CLOCK_SIGNAL);
           if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
               errExit("sigprocmask");

           /* Create the sample loop timer */
           loop_sev.sigev_notify = SIGEV_SIGNAL;
           loop_sev.sigev_signo = SAMPLE_CLOCK_SIGNAL;
           loop_sev.sigev_value.sival_ptr = &sample_loop_timerid;
           if (timer_create(CLOCK_MONOTONIC, &loop_sev, &sample_loop_timerid) == -1)
               errExit("timer_create");

           fprintf(stderr, "timer ID is 0x%lx\n", (long) sample_loop_timerid);

           /* Start the sample timer */
           loop_its.it_value.tv_sec = PERIOD_CLOCK_SAMPLE / 1000000000;
           loop_its.it_value.tv_nsec = PERIOD_CLOCK_SAMPLE % 1000000000;
           loop_its.it_interval.tv_sec = loop_its.it_value.tv_sec;
           loop_its.it_interval.tv_nsec = loop_its.it_value.tv_nsec;

           if (timer_settime(sample_loop_timerid, 0, &loop_its, NULL) == -1)
                errExit("timer_settime");


           /* Unblock timers */
           fprintf(stderr, "Unblocking signal %d\n", SAMPLE_CLOCK_SIGNAL);
           if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
               errExit("sigprocmask");
}


/* Voices */
#ifdef FASTVOICE
    const uint8_t max_voices=1;
    VoiceFast voices[max_voices]; //TODO: figure out who owns this initialization
#else
    const uint8_t max_voices=6;
    Voice voices[max_voices]; //TODO: figure out who owns this initialization
#endif

extern const uint8_t max_voices;
int main() {

    /* Install ctrl-C handler, need to restore serial port on quit */
    signal(SIGINT, ctrl_c);

    //midi_init();
    serial_port_setup();

#ifdef FASTVOICE
    voices[0].init(t_pulse);
    //voices[1].init(t_pulse);
    //voices[2].init(t_triangle);
#else
    voices[0].init(t_sin, fatty_base_instrument1);
    voices[1].init(t_pulse, flute_instrument2);
    voices[2].init(t_noise, drum_instrument1);

    voices[3].init(t_triangle, fatty_base_instrument1);

    voices[4].init(t_pulse,fatty_base_instrument1);
    voices[5].init(t_pulse,fatty_base_instrument1);
#endif

    setup_timer();

    for (;;) {
        sched_yield();
    }
}

static uint16_t mixer=0;
static uint16_t fast_timer=0;

void timer0_sample_loop(int sig, siginfo_t *si, void *uc) {

    /* Output signal first to avoid jitter? */
    uint8_t low_byte = (uint8_t)(mixer & 0xff);
    uint8_t hi_byte = (uint8_t)(mixer >> 8);
    fwrite(&low_byte,1,1,stdout);
    fwrite(&hi_byte,1,1,stdout);

    fast_timer++;

    process_midi_messages();   

    mixer=0;
    for (int i=0;i<max_voices;i++) {
        mixer += (voices[i].sample(fast_timer));
    }
     //mixer <<= 2;
     mixer <<= 4; //adjust volume (16bit dac, with only 12 bits)
}

void ctrl_c(int) {
    serial_port_teardown();
    fprintf(stderr, "fast_timer is %d\n",fast_timer);
    fprintf(stderr, "Quitting...\n");
    exit(0);

}
