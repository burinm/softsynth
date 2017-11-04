#!/bin/bash

./test/test_notes $1 | pv | mplayer -ao alsa \
                       -really-quiet \
                       -rawaudio channels=1:rate=11025:samplesize=2 \
                       -demuxer rawaudio - 
