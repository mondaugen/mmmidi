#ifndef MM_MIDIROUTER_H
#define MM_MIDIROUTER_H

#include "mm_midimsg.h" 

#define  MIDI_Router_NUM_CHANS 16;

typedef enum {
    MIDI_Router_Err_GOOD,
    MIDI_Router_Err_NOTIMP, /* Output when something is not implemented, such as the kind of message */
    MIDI_Router_Err_NULL    /* Something was NULL, like perhaps the message that was passed */
} MIDI_Router_Err;


typedef struct __MIDI_Router MIDI_Router;
typedef struct __MIDI_CB_Set MIDI_CB_Set;
typedef struct __MIDI_CB_Info MIDI_CB_Info;
typedef void (*MIDI_Router_CB)(void*,MIDIMsg_Byte_t,MIDIMsg_Byte_t);

struct __MIDI_CB_Info {
    MIDI_Router_CB callback;
    void *data;
};

struct __MIDI_CB_Set {
    MIDI_Router_CB noteOff;
    MIDI_Router_CB noteOn;
    MIDI_Router_CB aftertouch;
    MIDI_Router_CB controlChange;
    MIDI_Router_CB programChange;
    MIDI_Router_CB channelPressure;
    MIDI_Router_CB pitchBendChange;
    MIDI_Router_CB systemExclusive;
};

struct __MIDI_Router {
    MIDI_CB_Set cbSets[MIDI_Router_NUM_CHANS];
};

int MIDI_Router_handleMsg(MIDI_Router *router, MIDIMsg *msg)
{
    if((!msg) || (!msg->data)) {
        return MIDI_Router_Err_NULL;
    }
    MIDIMsg_Byte_t chan = MIDIMSG_GET_CHANNEL(msg->data[0]);
    switch (MIDIMSG_GET_STATUS(msg->data[0])) {
        case MIDIMSG_NOTE_OFF :
            router->cbSets[chan]->




#endif /* MM_MIDIROUTER_H */
