#ifndef __NOTE_POOL_TINY_H__
#define __NOTE_POOL_TINY_H__

/* 8 entry, tiny pool - currently developed for a polyphonic MIDI keypress 
    Pool stores uint8_t, item is 0-127, higher bits left alone for adding custom flags

    Adding the same value will not add it twice, so the remove short circuits on a find
    Adding is currently terribly inefficient
*/
#include <stdint.h>
#include "Envelope.h"

//#include <stdio.h> //remove after debugging

namespace SoftSynth {


#define POOL_TINY_SIZE 8 
#define POOL_TINY_ADD

#define POOL_TINY_MARK_FULL(b)        (empty_state |= (1<<b))
#define POOL_TINY_MARK_EMPTY(b)       (empty_state &= ~(1<<b))
#define POOL_TINY_IS_FULL(b)          (empty_state & (1<<b))

typedef struct {
    uint8_t note;
    Envelope envelope;
} note_t;


class NotePoolTiny {

    public:
        inline void init() {
                index=0;
                empty_state=0;

                for (uint8_t i=0;i<POOL_TINY_SIZE;i++) {
                    notes[i].note = 0;
                    notes[i].envelope.init();
                }
        }

        inline note_t* addValue(uint8_t v) {
            uint8_t count = 0;

            while (count < POOL_TINY_SIZE) {
                if (POOL_TINY_IS_FULL(count)) {
                    if ( notes[count].note == v) {
                        //Note already added
                        return &notes[count];
                    }
                }
                count++;
            }

            count = 0;
            while (count < POOL_TINY_SIZE) {
                if (POOL_TINY_IS_FULL(count) == 0) {
                    notes[count].note = v;
                    POOL_TINY_MARK_FULL(count);
                    return &notes[count];
                }
                count++;
            }
            return 0;
        }

        inline uint8_t removeValue(uint8_t v) {
            uint8_t found=0;
            uint8_t count = 0;

            while (count < POOL_TINY_SIZE) {
                if (POOL_TINY_IS_FULL(count)) {
                    if ( notes[count].note == v) {
                        POOL_TINY_MARK_EMPTY(count);
                        found++;
                        return found;
                    }
                }
                count++;
            }
            return found;
        }

        inline note_t* getValue(uint8_t v) {
            uint8_t count = 0;

            while (count < POOL_TINY_SIZE) {
                if (POOL_TINY_IS_FULL(count)) {
                    if ( notes[count].note == v) {
                        return &notes[count];
                    }
                }
                count++;
            }
            return 0;
        }

        inline note_t* getIterate() {

            if (index == POOL_TINY_SIZE) {
                index = 0;
                return 0;
            }

            while (index < POOL_TINY_SIZE) {
                if (POOL_TINY_IS_FULL(index)) {
                    return &notes[index++];
                }
                index++;
            }
            index=0;
            return 0;
        }

    private:

        note_t notes[POOL_TINY_SIZE];
        uint8_t empty_state;
        uint8_t index;
};


}
#endif
