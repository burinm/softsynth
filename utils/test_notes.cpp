#include <cstdio>
#include <cstdint>
#include <cstdlib>

//#include <csignal>
#include <ctime>

#include <cmath> //round
#include <sched.h> //yield

#include "test_midi.h"
#include "../Voice.h"
#include "../Envelope.h"
#include "../midi.h"
#include "../instruments.h"

extern "C" {
    #include "../wave_function.h"
}

using namespace SoftSynth;


void ctrl_c(int);
void timer0_sample_loop(int sig, siginfo_t *si, void *uc);

void setup_timer();


void setup_timer() {
#define FREQUENCY_SAMPLE_CLOCK  90703 //11025Hz

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
           loop_its.it_value.tv_sec = FREQUENCY_SAMPLE_CLOCK / 1000000000;
           loop_its.it_value.tv_nsec = FREQUENCY_SAMPLE_CLOCK % 1000000000;
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
extern const uint8_t max_voices;
const uint8_t max_voices=4;
Voice voices[max_voices]; //TODO: figure out who owns this initialization
//static uint16_t i=0;
int main(int argc, char* argv[]) {
uint8_t note = 57;

    if(argc == 2) {
        note = (uint8_t)atoi(argv[1]);
    }

    /* Install ctrl-C handler, need to restore serial port on quit */
    signal(SIGINT, ctrl_c);

    //midi_init();
    serial_port_setup();

    voices[0].init(t_sin,flute_instrument1);
    voices[1].init(t_pulse,flute_instrument2);
    voices[2].init(t_sawtooth,flute_instrument2);
    voices[3].init(t_triangle,flute_instrument1);

    setup_timer();

    for (;;) {
        sched_yield();
    }
}

static uint16_t mixer=0;
static uint16_t fast_timer=0;
static long prev_tone_clock;
struct timespec read_time;


void timer0_sample_loop(int sig, siginfo_t *si, void *uc) {

    /* Output signal first to avoid jitter? */
    uint8_t low_byte = (uint8_t)(mixer & 0xff);
    uint8_t hi_byte = (uint8_t)(mixer >> 8);
    fwrite(&low_byte,1,1,stdout);
    fwrite(&hi_byte,1,1,stdout);

    if (clock_gettime(CLOCK_MONOTONIC, &read_time) != 0) {
        fprintf(stderr, "Could not read CLOCK_MONOTONIC\n");
        ctrl_c(0); //TODO: send a signal here
    }

    long n_seconds;

    //This assumes that this Signal gets called within 1 second
    //TODO: start case when prev_tone_clock is 0
    if (read_time.tv_nsec - prev_tone_clock < 0 ) {
        n_seconds = read_time.tv_nsec + (1000000000 - prev_tone_clock);
    } else {
        n_seconds = read_time.tv_nsec - prev_tone_clock;
    }

    prev_tone_clock = read_time.tv_nsec;

    //TODO: This should be 16000, but that is flat? Why?
    // ( 1/(16MHz / 256)
    //           / 1E-09)
    long tmp_t = n_seconds / (double)15100;
    fast_timer += (uint16_t)round(tmp_t);

    process_midi_messages();   

    mixer=0;
    for (int i=0;i<max_voices;i++) {
        voices[i].step(fast_timer);
        mixer += (voices[i].sample());
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
