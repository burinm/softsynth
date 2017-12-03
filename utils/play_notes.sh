#!/bin/bash


#./test/test_notes $1 | pv | mplayer -ao alsa \
#                       -really-quiet \
#                       -rawaudio channels=1:rate=11025:samplesize=2 \
#                       -demuxer rawaudio -

#Works, but terrible latency
#./test/test_notes $1 |  mplayer -really-quiet -ao alsa -rawaudio channels=1:rate=11025:samplesize=2 -demuxer rawaudio -
#./test/test_notes $1 |  mplayer -really-quiet -ao jack -rawaudio channels=1:rate=11025:samplesize=2 -demuxer rawaudio -

#Works, a little choppy, but better latency
#./test/test_notes | ffplay -vn -fflags nobuffer -f s16le -ar 11025 -ac 1 -

#Works the best so far
#chrt -f 21 ./test/test_notes  | aplay --rate 11025 --format S16_LE --channels 1 -

#Adjust for different sample rate on Linux version
#chrt -f 21 ./test/test_notes  | aplay --rate 15625 --format S16_LE --channels 1 -
chrt -f 21 ./test/test_notes  | aplay --rate 17000 --format S16_LE --channels 1 -
#chrt -f 21 ./test/test_notes  | aplay --rate 31250 --format S16_LE --channels 1 -
