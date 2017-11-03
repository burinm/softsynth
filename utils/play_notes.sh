#!/bin/bash

./test/test_notes $1 | mplayer -ao alsa -volume 25 \
                       -rawaudio channels=1:rate=11025:samplesize=1 \
                       -demuxer rawaudio - 
