#!/bin/bash


#./test/test_notes $1 | pv | mplayer -ao alsa \
#                       -really-quiet \
#                       -rawaudio channels=1:rate=11025:samplesize=2 \
#                       -demuxer rawaudio -

#Works, but terrible latency
#./test/test_notes $1 |  mplayer -ao alsa -rawaudio channels=1:rate=11025:samplesize=2 -demuxer rawaudio -

#Works, a little choppy, but better latency
./test/test_notes | ffplay -vn -fflags nobuffer -f s16le -ar 11025 -ac 1 -
