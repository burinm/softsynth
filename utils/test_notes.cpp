#include <cstdio>
#include <cstdint>
#include <cstdlib>

//#include <csignal>
#include <ctime>

#include <sched.h> //yield

#include "../Voice.h"
#include "../Envelope.h"
#include "../midi.h"

extern "C" {
    #include "test_midi.h"
    #include "../wave_function.h"
}

using namespace SoftSynth;

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

void ctrl_c(int);
void timer0_pop(int sig, siginfo_t *si, void *uc);

void setup_timer();


void setup_timer() {
#define FREQUENCY_NANO  90703 //11025Hz

/* Copied almost verbatim from timer_create(2) man page
    Linux Programmer's Manual
*/
#define CLOCKID CLOCK_REALTIME

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

static timer_t timerid;
static struct sigevent sev;
static struct itimerspec its;
static long long freq_nanosecs;
static sigset_t mask;
static struct sigaction sa;

           /* Establish handler for timer signal */

           fprintf(stderr, "Establishing handler for signal %d\n", OSC_CLOCK_SIGNAL);
           sa.sa_flags = SA_SIGINFO;
           sa.sa_sigaction = timer0_pop;
           //sigemptyset(&sa.sa_mask);
           sigaddset(&sa.sa_mask,MIDI_INPUT_SIGNAL); //Block I/O (serial) midi signals
           if (sigaction(OSC_CLOCK_SIGNAL, &sa, NULL) == -1)
               errExit("sigaction");

           /* Block timer signal temporarily */

           fprintf(stderr, "Blocking signal %d\n", OSC_CLOCK_SIGNAL);
           sigemptyset(&mask);
           sigaddset(&mask, OSC_CLOCK_SIGNAL);
           if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
               errExit("sigprocmask");

           /* Create the timer */

           sev.sigev_notify = SIGEV_SIGNAL;
           sev.sigev_signo = OSC_CLOCK_SIGNAL;
           sev.sigev_value.sival_ptr = &timerid;
           if (timer_create(CLOCKID, &sev, &timerid) == -1)
               errExit("timer_create");

           fprintf(stderr, "timer ID is 0x%lx\n", (long) timerid);

           /* Start the timer */

           its.it_value.tv_sec = FREQUENCY_NANO / 1000000000;
           its.it_value.tv_nsec = FREQUENCY_NANO % 1000000000;
           its.it_interval.tv_sec = its.it_value.tv_sec;
           its.it_interval.tv_nsec = its.it_value.tv_nsec;

           if (timer_settime(timerid, 0, &its, NULL) == -1)
                errExit("timer_settime");

           /* Sleep for a while; meanwhile, the timer may expire
              multiple times */

           fprintf(stderr, "Unblocking signal %d\n", OSC_CLOCK_SIGNAL);
           if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
               errExit("sigprocmask");
}

static uint8_t sample;

/* Voices */
extern const uint8_t max_voices;
const uint8_t max_voices=5;
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

#if 0
    for (int i=0;i<max_voices;i++) {
        //voices[i].init(t_sin,flute_instrument);
        voices[i].init(t_sawtooth,flute_instrument);
    }
#endif

    voices[0].init(t_pulse,flute_instrument);
    voices[1].init(t_sin,flute_instrument);
    voices[2].init(t_triangle,flute_instrument);
    voices[3].init(t_sawtooth,flute_instrument);
    voices[4].init(t_noise,flute_instrument);

//    voices[0].startNote(note);

    setup_timer();

    for (;;) {
        sched_yield();
    }
}

static uint16_t mixer=0;
void timer0_pop(int sig, siginfo_t *si, void *uc) {

    uint8_t low_byte = (uint8_t)(mixer &0xff);
    uint8_t hi_byte = (uint8_t)(mixer >> 8);
    fwrite(&low_byte,1,1,stdout);
    fwrite(&hi_byte,1,1,stdout);
    
    process_midi_messages();   

    mixer=0;
    for (int i=0;i<max_voices;i++) {
        voices[i].step();
        mixer += (voices[i].sample());
    }
     //mixer <<= 2;
     mixer <<= 4; //adjust volume (16bit dac, with only 12 bits)

  //  v0.step();
  //  sample = v0.sample();
 //   sample = v0.sample();
  //  sample = (255-sample);
   // fwrite(&mixer,1,1,stdout);
}

void ctrl_c(int) {
    serial_port_teardown();
    fprintf(stderr, "Quitting...\n");
    exit(0);

}
