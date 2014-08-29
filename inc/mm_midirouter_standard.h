#ifndef MM_MIDIROUTER_STANDARD_H
#define MM_MIDIROUTER_STANDARD_H

#include "mm_midirouter.h"
#include "mm_midiccrouter.h" 

typedef struct __MIDI_Router_Standard MIDI_Router_Standard;

struct __MIDI_Router_Standard {
    MIDI_Router router;
    MIDI_CC_CB_Router cbRouters[MIDI_Router_NUM_CHANS];
};

void MIDI_Router_Standard_init(MIDI_Router_Standard *mr);

#endif /* MM_MIDIROUTER_STANDARD_H */
