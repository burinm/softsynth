#include "test_midi.h"
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/signal.h>
#include <sys/types.h>

extern "C" {
#include "../circbuf_tiny.h"
}

extern circbuf_tiny_t midi_buf;

#include "../debug.h"
#include "../midi.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS0"
#define TRUE 1


static int serialp_fd;
static struct termios oldtio;
static struct termios newtio;
static struct sigaction saio;           /* definition of signal action */
void signal_handler_IO (int status);


void serial_port_setup() {

        midi_init();
        
/* Modified from:
     http://www.faqs.org/docs/Linux-HOWTO/Serial-Programming-HOWTO.html
*/

        /* open the device to be non-blocking (read will return immediatly) */
        serialp_fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK); //O_ASYNC doesn't work?
        if (serialp_fd <0) {perror(MODEMDEVICE); exit(-1); }
        
        /* install the signal handler before making the device asynchronous */
        saio.sa_handler = signal_handler_IO;
        //sigemptyset(&saio.sa_mask);
        sigaddset(&saio.sa_mask, SAMPLE_CLOCK_SIGNAL); //block all other clock signals 
        saio.sa_flags = 0;
        saio.sa_restorer = NULL;
        if (sigaction(MIDI_INPUT_SIGNAL,&saio,NULL) != 0) {
           perror("Couldn't install I/O signal handler");
           exit(-1); 
        }

        /* allow the process to receive SIGIO */
        fcntl(serialp_fd, F_SETOWN, getpid());

        /* Remap I/O signal to real time signal so priority can be controlled */
       fcntl(serialp_fd, F_SETSIG, MIDI_INPUT_SIGNAL);

        /* Make the file descriptor asynchronous (the manual page says only 
           O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
       fcntl(serialp_fd, F_SETFL, FASYNC);
        
        tcgetattr(serialp_fd,&oldtio); /* save current port settings */
        /* set new port settings for canonical input processing */

        //Hardware flow control off
        newtio.c_cflag &= ~CRTSCTS;

        //Ignore modem control lines
        newtio.c_cflag |= CLOCAL;

        //38400 baud
        //newtio.c_cflag |= BAUDRATE;
        if (cfsetispeed(&newtio,BAUDRATE) != 0) {
            perror("Couldn't set serial speed\n");
            exit(-1);
        }

        //8n1
        newtio.c_cflag &= ~PARENB;  //Parity off
        newtio.c_cflag &= ~CSTOPB;  //Turn off 2 stop bits
        newtio.c_cflag &= ~CSIZE;   //clear character size mask
        newtio.c_cflag |= CS8;      //set 8 bits

        //Enable receiver
        newtio.c_cflag |= CREAD;

        // With O_NONBLOCK these don't matter
        newtio.c_cc[VMIN]=0;        //1 byte only
        newtio.c_cc[VTIME]=0;
        
        //cfmakeraw(&newtio);
        newtio.c_iflag |= (IGNBRK | IGNPAR | IGNCR );
        newtio.c_iflag &= ~(INPCK | IXOFF);
        newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        tcflush(serialp_fd, TCIFLUSH);
        tcsetattr(serialp_fd,TCSANOW,&newtio);
}

void serial_port_teardown() {
    tcsetattr(serialp_fd,TCSANOW,&oldtio);
}

void signal_handler_IO (int status) {
uint8_t b[255]; // TODO: select buffer size, 1?
ssize_t read_count=0;    

        while(1) {
                read_count = read(serialp_fd,&b,1);
                if (read_count == -1) {
                    if (errno == EINTR || errno == EAGAIN) {
                        continue;
                    } else {
                        break;
                    }
                } else {
                    if (read_count == 0) {
                        break;
                    } else {
                        for (int i=0;i<read_count;i++) {
                            if (circbuf_tiny_write(&midi_buf,b[i]) == 0) {
                                ERROR_SET(ERROR_OVERFLOW);
                            }
                        }
                    }
                }
        }
}
