#include <cstdio>
#include <cstdint>
#include <cstdlib>

//#include <csignal>
#include <ctime>

//#include <sched.h> //yield

#include "../Voice.h"
#include "../Envelope.h"
#include "../midi.h"

extern "C" {
    #include "test_midi.h"
    #include "../wave_function.h"
}

using namespace SoftSynth;

const uint16_t note_phase_mult_table[128] = {
        49,    52,    55,    58,    61,    65,    69,    73,    77,    82,    87,    92,
        97,   103,   109,   116,   123,   130,   138,   146,   154,   164,   173,   184,
       194,   206,   218,   231,   245,   260,   275,   291,   309,   327,   347,   367,
       389,   412,   437,   463,   490,   519,   550,   583,   617,   654,   693,   734,
       778,   824,   873,   925,   980,  1038,  1100,  1166,  1235,  1308,  1386,  1469,
      1556,  1649,  1747,  1850,  1960,  2077,  2201,  2331,  2470,  2617,  2772,  2937,
      3112,  3297,  3493,  3701,  3921,  4154,  4401,  4663,  4940,  5234,  5545,  5875,
      6224,  6594,  6986,  7402,  7842,  8308,  8802,  9325,  9880, 10467, 11090, 11749,
     12448, 13188, 13972, 14803, 15683, 16616, 17604, 18651, 19760, 20935, 22180, 23499,
     24896, 26376, 27945, 29606, 31367,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0
};

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
           sigaddset(&sa.sa_mask,SIGIO); //Block I/O (serial) signals
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
extern Voice voices[MAX_VOICES]; //TODO: figure out who owns this initialization
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

    for (int i=0;i<MAX_VOICES;i++) {
        //voices[i].init(t_sin,flute_instrument);
        voices[i].init(t_sawtooth,flute_instrument);
    }
    //v0.init(t_sin,flute_instrument);
    voices[0].startNote(note);

    setup_timer();

    for (;;) {
        //sched_yield();
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
    for (int i=0;i<MAX_VOICES;i++) {
        voices[i].step();
        mixer += (voices[i].sample());
    }
     mixer <<= 2;

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
