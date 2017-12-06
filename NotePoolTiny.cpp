#ifndef __NOTE_POOL_TINY_H__
#define __NOTE_POOL_TINY_H__

/* 8 entry, tiny pool - currently developed for a polyphonic MIDI keypress 
    Pool stores uint8_t, item is 0-127, higher bits left alone for adding custom flags
*/
#include <stdint.h>
#include "Envelope.h"

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
                empty_state=0;

                for (uint8_t i=0;i<POOL_TINY_SIZE;i++) {
                    notes[i].note = 0;
                    notes[i].envelope.init();
                }
        }

        inline note_t* pool_tiny_add(uint8_t v) {
            uint8_t count = 0;

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

        inline void pool_tiny_remove(uint8_t v) {
            uint8_t count = 0;

            while (count < POOL_TINY_SIZE) {
                if ( notes[count].note == v) {
                    if (POOL_TINY_IS_FULL(count)) {
                        POOL_TINY_MARK_EMPTY(count);
                        break;
                    }
                }
                count++;
            }
        }

        inline note_t* pool_tiny_get(uint8_t v) {
            uint8_t count = 0;

            while (count < POOL_TINY_SIZE) {
                if ( notes[count].note == v) {
                    if (POOL_TINY_IS_FULL(count)) {
                        return &notes[count];
                    }
                }
                count++;
            }
            return 0;
        }

        inline note_t* pool_tiny_get_all() {
            uint8_t count = 0;

            while (count < POOL_TINY_SIZE) {
                if (POOL_TINY_IS_FULL(count)) {
                    return &notes[count];
                }
                count++;
            }
            return 0;
        }

    private:

        note_t notes[POOL_TINY_SIZE];
        uint8_t empty_state;
};


}
#endif
